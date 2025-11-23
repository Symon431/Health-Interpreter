# Invoice Management System

A starter codebase for managing customers, invoices (including proforma), payments, and balances for a small business. The stack uses **FastAPI** for APIs and Python data models to demonstrate the workflow end-to-end. This repository is meant as a foundation you can evolve into a production system (with real databases, payment integrations, and messaging providers).

## Features (MVP)
- Customer accounts with basic profile information.
- Invoices with line items, totals, due dates, and statuses (draft/sent/paid/overdue).
- Proforma invoice support (flagged invoices) that can be converted to standard invoices.
- Payment records (M-Pesa, bank transfer, manual) that update customer balances.
- Balance calculation derived from a ledger of charges and credits.
- Simple notifications hook to plug in email/SMS providers later.

## Project structure
```
backend/
  app/
    main.py             # FastAPI app and routers
    models.py           # Pydantic schemas and in-memory data stores
    ledger.py           # Balance and ledger utilities
  requirements.txt      # Python dependencies
.gitignore
```

## Getting started
1. **Setup environment**
   ```bash
   cd backend
   python -m venv .venv
   source .venv/bin/activate
   pip install -r requirements.txt
   ```

2. **Run the API server**
   ```bash
   uvicorn app.main:app --reload
   ```
   The OpenAPI docs will be available at http://127.0.0.1:8000/docs.

3. **Sample workflow**
   - Create a customer: `POST /customers`
   - Create a proforma invoice with line items: `POST /invoices`
   - Convert to a standard invoice: `POST /invoices/{id}/convert`
   - Record a payment (e.g., M-Pesa): `POST /payments`
   - Check the customer balance and ledger: `GET /customers/{id}`

## Next steps (recommendations)
- Replace in-memory storage with a real database (e.g., PostgreSQL + SQLAlchemy/Prisma).
- Integrate M-Pesa via Daraja or a gateway (Flutterwave/Paystack) with webhook validation.
- Add email/SMS notifications using SendGrid/Mailgun/AfricasTalking.
- Implement authentication and role-based access control for owners vs. customers.
- Add audit logging, reporting (aging, top debtors), and dashboards.
- Containerize the services and wire CI/CD for deployment.

## License
MIT
