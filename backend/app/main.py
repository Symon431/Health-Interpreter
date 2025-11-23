from datetime import datetime
import uuid
from typing import List

from fastapi import FastAPI, HTTPException

from .ledger import add_ledger_entry, compute_balance, normalize_due_status
from .models import (
    Customer,
    CustomerCreate,
    Invoice,
    InvoiceCreate,
    LedgerEntry,
    LedgerEntryType,
    LineItem,
    Payment,
    PaymentCreate,
    PaymentMethod,
    CUSTOMERS,
    INVOICES,
    PAYMENTS,
    LEDGER,
)

app = FastAPI(title="Invoice Management System", version="0.1.0")


@app.get("/health", tags=["health"])
def healthcheck():
    return {"status": "ok", "timestamp": datetime.utcnow()}


@app.post("/customers", response_model=Customer, tags=["customers"], status_code=201)
def create_customer(payload: CustomerCreate):
    customer_id = str(uuid.uuid4())
    customer = Customer(
        id=customer_id,
        name=payload.name,
        email=payload.email,
        phone=payload.phone,
        account_number=payload.account_number,
        balance=0.0,
        ledger=[],
    )
    CUSTOMERS[customer_id] = customer
    return customer


@app.get("/customers", response_model=List[Customer], tags=["customers"])
def list_customers():
    customers: List[Customer] = []
    for customer in CUSTOMERS.values():
        balance = compute_balance(LEDGER, customer.id)
        customers.append(customer.copy(update={"balance": balance, "ledger": LEDGER.get(customer.id, [])}))
    return customers


@app.get("/customers/{customer_id}", response_model=Customer, tags=["customers"])
def get_customer(customer_id: str):
    customer = CUSTOMERS.get(customer_id)
    if not customer:
        raise HTTPException(status_code=404, detail="Customer not found")
    balance = compute_balance(LEDGER, customer_id)
    return customer.copy(update={"balance": balance, "ledger": LEDGER.get(customer_id, [])})


@app.post("/invoices", response_model=Invoice, tags=["invoices"], status_code=201)
def create_invoice(payload: InvoiceCreate):
    if payload.customer_id not in CUSTOMERS:
        raise HTTPException(status_code=404, detail="Customer not found")

    invoice_id = str(uuid.uuid4())
    total = sum(item.total for item in payload.line_items)
    status = "draft" if payload.proforma else "sent"
    invoice = Invoice(
        id=invoice_id,
        customer_id=payload.customer_id,
        due_date=payload.due_date,
        status=status,
        proforma=payload.proforma,
        line_items=payload.line_items,
        total=round(total, 2),
    )
    INVOICES[invoice_id] = invoice

    entry_type = LedgerEntryType.PROFORMA if payload.proforma else LedgerEntryType.CHARGE
    add_ledger_entry(
        LEDGER,
        LedgerEntry(
            customer_id=payload.customer_id,
            entry_type=entry_type,
            amount=invoice.total,
            description=("Proforma invoice" if payload.proforma else "Invoice"),
            related_id=invoice_id,
        ),
    )

    return invoice


@app.post("/invoices/{invoice_id}/convert", response_model=Invoice, tags=["invoices"])
def convert_proforma(invoice_id: str):
    invoice = INVOICES.get(invoice_id)
    if not invoice:
        raise HTTPException(status_code=404, detail="Invoice not found")
    if not invoice.proforma:
        raise HTTPException(status_code=400, detail="Invoice is already standard")

    invoice = invoice.copy(update={"proforma": False, "status": "sent"})
    INVOICES[invoice_id] = invoice
    add_ledger_entry(
        LEDGER,
        LedgerEntry(
            customer_id=invoice.customer_id,
            entry_type=LedgerEntryType.CHARGE,
            amount=invoice.total,
            description="Converted proforma to invoice",
            related_id=invoice_id,
        ),
    )
    return invoice


@app.get("/invoices", response_model=List[Invoice], tags=["invoices"])
def list_invoices():
    normalized = []
    for invoice in INVOICES.values():
        normalized.append(
            invoice.copy(update={"status": normalize_due_status(invoice.due_date, invoice.status)})
        )
    return normalized


@app.get("/invoices/{invoice_id}", response_model=Invoice, tags=["invoices"])
def get_invoice(invoice_id: str):
    invoice = INVOICES.get(invoice_id)
    if not invoice:
        raise HTTPException(status_code=404, detail="Invoice not found")
    return invoice.copy(update={"status": normalize_due_status(invoice.due_date, invoice.status)})


@app.post("/payments", response_model=Payment, tags=["payments"], status_code=201)
def record_payment(payload: PaymentCreate):
    if payload.customer_id not in CUSTOMERS:
        raise HTTPException(status_code=404, detail="Customer not found")
    if payload.invoice_id and payload.invoice_id not in INVOICES:
        raise HTTPException(status_code=404, detail="Invoice not found")

    payment_id = str(uuid.uuid4())
    payment = Payment(
        id=payment_id,
        created_at=datetime.utcnow(),
        customer_id=payload.customer_id,
        invoice_id=payload.invoice_id,
        amount=payload.amount,
        method=payload.method,
        reference=payload.reference,
    )
    PAYMENTS[payment_id] = payment

    description = f"Payment via {payload.method.value}"
    add_ledger_entry(
        LEDGER,
        LedgerEntry(
            customer_id=payload.customer_id,
            entry_type=LedgerEntryType.PAYMENT,
            amount=payload.amount,
            description=description,
            related_id=payment_id,
        ),
    )

    if payload.invoice_id:
        invoice = INVOICES[payload.invoice_id]
        updated_status = "paid" if payload.amount >= invoice.total else invoice.status
        INVOICES[payload.invoice_id] = invoice.copy(update={"status": updated_status})

    return payment


@app.post("/adjustments", response_model=LedgerEntry, tags=["ledger"], status_code=201)
def create_adjustment(customer_id: str, amount: float, reason: str):
    if customer_id not in CUSTOMERS:
        raise HTTPException(status_code=404, detail="Customer not found")
    entry = LedgerEntry(
        customer_id=customer_id,
        entry_type=LedgerEntryType.ADJUSTMENT,
        amount=amount,
        description=reason,
    )
    add_ledger_entry(LEDGER, entry)
    return entry


@app.get("/ledger/{customer_id}", response_model=List[LedgerEntry], tags=["ledger"])
def get_ledger(customer_id: str):
    if customer_id not in CUSTOMERS:
        raise HTTPException(status_code=404, detail="Customer not found")
    return LEDGER.get(customer_id, [])
