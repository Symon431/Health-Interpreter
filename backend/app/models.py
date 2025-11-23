from datetime import datetime
from enum import Enum
from typing import List, Dict, Optional

from pydantic import BaseModel, Field


class LedgerEntryType(str, Enum):
    CHARGE = "charge"
    PROFORMA = "proforma"
    PAYMENT = "payment"
    ADJUSTMENT = "adjustment"


class PaymentMethod(str, Enum):
    MPESA = "mpesa"
    BANK_TRANSFER = "bank_transfer"
    MANUAL = "manual"


class CustomerCreate(BaseModel):
    name: str
    email: str
    phone: Optional[str] = None
    account_number: Optional[str] = None


class Customer(BaseModel):
    id: str
    name: str
    email: str
    phone: Optional[str]
    account_number: Optional[str]
    balance: float = 0.0
    ledger: List["LedgerEntry"] = Field(default_factory=list)


class LineItem(BaseModel):
    description: str
    quantity: float = Field(gt=0)
    unit_price: float = Field(gt=0)

    @property
    def total(self) -> float:
        return round(self.quantity * self.unit_price, 2)


class InvoiceCreate(BaseModel):
    customer_id: str
    due_date: datetime
    proforma: bool = False
    line_items: List[LineItem]


class Invoice(BaseModel):
    id: str
    customer_id: str
    due_date: datetime
    status: str
    proforma: bool = False
    line_items: List[LineItem]
    total: float


class PaymentCreate(BaseModel):
    customer_id: str
    invoice_id: Optional[str] = None
    amount: float = Field(gt=0)
    method: PaymentMethod
    reference: Optional[str] = None


class Payment(BaseModel):
    id: str
    created_at: datetime
    customer_id: str
    invoice_id: Optional[str]
    amount: float
    method: PaymentMethod
    reference: Optional[str]


class LedgerEntry(BaseModel):
    customer_id: str
    entry_type: LedgerEntryType
    amount: float
    description: str
    related_id: Optional[str] = None
    created_at: datetime = Field(default_factory=datetime.utcnow)


# In-memory stores
CUSTOMERS: Dict[str, Customer] = {}
INVOICES: Dict[str, Invoice] = {}
PAYMENTS: Dict[str, Payment] = {}
LEDGER: Dict[str, List[LedgerEntry]] = {}
