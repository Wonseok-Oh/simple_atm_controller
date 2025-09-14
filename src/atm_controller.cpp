#include "simple_atm_controller/atm_controller.hpp"

ATMController::ATMController(std::shared_ptr<IBankService> bank_service) : m_CurrentState(State::IDLE), m_pBankService(bank_service)
{
    // The constructor expects a valid bank_service pointer.
    if (!m_pBankService)
    {
        throw std::invalid_argument("Bank service cannot be null");
    }
}

bool ATMController::insertCardAndVerifyPin(const Card &card, const std::string &pin)
{
    // A new session starts, so reset the internal state
    m_CurrentState = State::CARD_INSERTED;
    m_CurrentCard = card;

    // Delegate PIN verification to the bank service
    bool isVerified = m_pBankService->verifyPin(m_CurrentCard, pin);

    if (isVerified)
    {
        m_CurrentState = State::PIN_VERIFIED;
        return true;
    }
    else
    {
        // If verification fails, reset to the initial state.
        m_CurrentState = State::IDLE;
        return false;
    }
}

std::vector<Account> ATMController::getAccounts()
{
    // State check: This operation is only allowed after PIN is verified.
    if (m_CurrentState != State::PIN_VERIFIED && m_CurrentState != State::ACCOUNT_SELECTED)
    {
        throw std::runtime_error("PIN not verified. Cannot get accounts");
    }

    return m_pBankService->getAccounts(m_CurrentCard);
}

void ATMController::selectAccount(const Account &account)
{
    // State check: This operation is only allowed after PIN is verified.
    if (m_CurrentState != State::PIN_VERIFIED && m_CurrentState != State::ACCOUNT_SELECTED)
    {
        throw std::runtime_error("PIN not verified. Cannot get accounts");
    }

    m_SelectedAccount = account;
    m_CurrentState = State::ACCOUNT_SELECTED;
}

Account ATMController::getSelectedAccount() const
{
    return m_SelectedAccount;
}

int ATMController::seeBalance()
{
    // State check: An account must be selected.
    if (m_CurrentState != State::ACCOUNT_SELECTED)
    {
        throw std::runtime_error("Account not selected. Cannot check balance.");
    }

    // Update local balance information from the authoritative source (bank service)
    m_SelectedAccount.balance = m_pBankService->getBalance(m_SelectedAccount.accountId);
    return m_SelectedAccount.balance;
}

bool ATMController::deposit(int amount)
{
    // State check: An account must be selected.
    if (m_CurrentState != State::ACCOUNT_SELECTED)
    {
        throw std::runtime_error("Account not selected. Cannot deposit.");
    }
    // Input validation
    if (amount <= 0)
    {
        return false; // Cannot deposit zero or a negative amount.
    }

    bool success = m_pBankService->deposit(m_SelectedAccount.accountId, amount);
    if (success)
    {
        // For consistency, update the local copy of the balance.
        m_SelectedAccount.balance += amount;
    }
    return success;
}

bool ATMController::withdraw(int amount)
{
    // State check: An account must be selected.
    if (m_CurrentState != State::ACCOUNT_SELECTED)
    {
        throw std::runtime_error("Account not selected. Cannot withdraw.");
    }
    // Input validation
    if (amount <= 0)
    {
        return false; // Cannot withdraw zero or a negative amount.
    }

    bool success = m_pBankService->withdraw(m_SelectedAccount.accountId, amount);
    if (success)
    {
        // For consistency, update the local copy of the balance.
        m_SelectedAccount.balance -= amount;
    }
    return success;
}