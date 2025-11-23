from datetime import datetime
from typing import List, Dict

from .models import LedgerEntry, LedgerEntryType


def add_ledger_entry(ledger: Dict[str, List[LedgerEntry]], entry: LedgerEntry) -> None:
    customer_ledger = ledger.setdefault(entry.customer_id, [])
    customer_ledger.append(entry)


def compute_balance(ledger: Dict[str, List[LedgerEntry]], customer_id: str) -> float:
    total = 0.0
    for entry in ledger.get(customer_id, []):
        if entry.entry_type in {LedgerEntryType.CHARGE, LedgerEntryType.PROFORMA}:
            total += entry.amount
        elif entry.entry_type == LedgerEntryType.PAYMENT:
            total -= entry.amount
        elif entry.entry_type == LedgerEntryType.ADJUSTMENT:
            total += entry.amount
    return round(total, 2)


def normalize_due_status(due_date: datetime, status: str) -> str:
    if status in {"paid", "cancelled"}:
        return status
    return "overdue" if due_date.date() < datetime.utcnow().date() else status
