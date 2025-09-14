#ifndef SIMPLE_ATM_CONTROLLER__ATM_CONTROLLER_HPP_
#define SIMPLE_ATM_CONTROLLER__ATM_CONTROLLER_HPP_

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

/**
 * @brief Simple struct for card info.
 *
 */
struct Card
{
    std::string cardNumber;

    // for gmock to compare object
    bool operator==(const Card &other) const
    {
        return cardNumber == other.cardNumber;
    }
};

/**
 * @brief Simple struct for account info.
 *
 */
struct Account
{
    std::string accountId;
    int balance;
};

/**
 * @class IBankService
 * @brief An interface for communicating with the bank system.
 * @details The ATM controller communicates with the bank system through this class.
 * A real network communication class or a mock class for testing should implement this interface.
 *
 */
class IBankService
{
public:
    // virtual destructor is necessary for interface
    virtual ~IBankService() = default;

    virtual bool verifyPin(const Card &card, const std::string &pin) = 0;
    virtual std::vector<Account> getAccounts(const Card &card) = 0;
    virtual int getBalance(const std::string &accountId) = 0;
    virtual bool deposit(const std::string &accountId, int amount) = 0;
    virtual bool withdraw(const std::string &accountId, int amount) = 0;
};

class ATMController
{
public:
    /**
     * @brief Construct a new ATMController object
     *
     * @param bankService A shared pointer to an object that implements IBankService interface.
     */
    explicit ATMController(std::shared_ptr<IBankService> bankService);

    /**
     * @brief The first step where a user inserts a card and enters a PIN.
     *
     * @param card The inserted card object.
     * @param pin The PIN entered by the user.
     * @return true if PIN verification is successful
     * @return false otherwise
     */
    bool insertCardAndVerifyPin(const Card &card, const std::string &pin);

    /**
     * @brief Retrieves the list of accounts for the authenticated card.
     *
     * @return std::vector<Account> A vector of Account object
     * @throw std::runtime_error Throws an exception if called before PIN verification
     */
    std::vector<Account> getAccounts();

    /**
     * @brief Selects an account for transactions.
     *
     * @param account The selected Account object.
     */
    void selectAccount(const Account &account);

    /**
     * @brief Get the Selected Account object
     *
     * @return The selected Account object
     */
    Account getSelectedAccount() const;

    /**
     * @brief Checks the balance of the selected account.
     *
     * @return int The balance as an integer.
     * @throw std::runtime_error Throws an exception if called before an account is selected.
     */
    int seeBalance();

    /**
     * @brief Deposits money into the selected account.
     *
     * @param amount The amount to deposit.
     * @return true if the deposit is successful,
     * @return false otherwise.
     */
    bool deposit(int amount);

    /**
     * @brief Withdraws money from the selected account.
     *
     * @param amount The amount to withdraw
     * @return true if the withdrawal is successful,
     * @return false otherwise.
     */
    bool withdraw(int amount);

private:
    // Enum to manage the current state of the ATM session
    enum class State
    {
        IDLE,            // Wating for a card
        CARD_INSERTED,   // Card has been inserted
        PIN_VERIFIED,    // PIN has been verified
        ACCOUNT_SELECTED // Account ahs been selected
    };

    // current state of the ATM session
    State m_CurrentState;

    // for communicating with the bank services
    std::shared_ptr<IBankService> m_pBankService;

    // The card being used in the current session
    Card m_CurrentCard;

    // The account selected in the current session
    Account m_SelectedAccount;
};

#endif // SIMPLE_ATM_CONTROLLER__ATM_CONTROLLER_HPP_