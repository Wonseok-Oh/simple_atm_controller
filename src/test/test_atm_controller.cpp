/**
 * @file test_atm_controller.cpp
 * @author Wonseok Oh (ooww0123@gmail.com)
 * @brief Test code for atm controller
 * @version 1.0
 * @date 2025-09-14
 *
 * @copyright Copyright (c) 2025
 *
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <vector>
#include <string>
#include <memory>

#include "simple_atm_controller/atm_controller.hpp"

// Mock bank service using Gmock
class MockBankService : public IBankService
{
public:
    MOCK_METHOD(bool, verifyPin, (const Card &card, const std::string &pin), (override));
    MOCK_METHOD(std::vector<Account>, getAccounts, (const Card &card), (override));
    MOCK_METHOD(int, getBalance, (const std::string &accountId), (override));
    MOCK_METHOD(bool, deposit, (const std::string &accountId, int amount), (override));
    MOCK_METHOD(bool, withdraw, (const std::string &accountId, int amount), (override));
};

// Test Fixture setting
class ATMControllerTest : public ::testing::Test
{
protected:
    Card m_TestCard{"1234"};
    Account m_TestAccount1{"ACCT-001", 100};
    Account m_TestAccount2{"ACCT-002", 500};

    // Mock objects and controller pointer
    std::shared_ptr<MockBankService> m_pMockBankService;
    std::unique_ptr<ATMController> m_pAtmController;

    void SetUp() override
    {
        // Generate Mock object
        m_pMockBankService = std::make_shared<MockBankService>();
        m_pAtmController = std::make_unique<ATMController>(m_pMockBankService);
    }
};

// testcases

// Scenario 1: Success from card insertion to withdraw
TEST_F(ATMControllerTest, SuccessfulFullTransactionFlow)
{
    using ::testing::Return;

    // step 1 & 2: card insertion & verify pin
    EXPECT_CALL(*m_pMockBankService, verifyPin(m_TestCard, "1234")).WillOnce(Return(true));

    ASSERT_TRUE(m_pAtmController->insertCardAndVerifyPin(m_TestCard, "1234"));

    // step 3: make inquiry for accounts list and select one
    EXPECT_CALL(*m_pMockBankService, getAccounts(m_TestCard)).WillOnce(Return(std::vector<Account>{m_TestAccount1, m_TestAccount2}));

    auto accounts = m_pAtmController->getAccounts();
    ASSERT_EQ(accounts.size(), 2);

    m_pAtmController->selectAccount(m_TestAccount1);
    ASSERT_EQ(m_pAtmController->getSelectedAccount().accountId, "ACCT-001");

    // step 4: balance inquiry
    EXPECT_CALL(*m_pMockBankService, getBalance("ACCT-001")).WillOnce(Return(100));

    ASSERT_EQ(m_pAtmController->seeBalance(), 100);

    // step 5: deposit
    EXPECT_CALL(*m_pMockBankService, deposit("ACCT-001", 50)).WillOnce(Return(true));
    ASSERT_TRUE(m_pAtmController->deposit(50));

    // step 6: withdraw
    EXPECT_CALL(*m_pMockBankService, withdraw("ACCT-001", 30)).WillOnce(Return(true));
    ASSERT_TRUE(m_pAtmController->withdraw(30));
}

// Scenario 2: Wrong PIN input
TEST_F(ATMControllerTest, InvalidPin)
{
    using ::testing::Return;

    // if a user call verifyPin with PIN "9999", return false
    EXPECT_CALL(*m_pMockBankService, verifyPin(m_TestCard, "9999")).WillOnce(Return(false));

    ASSERT_FALSE(m_pAtmController->insertCardAndVerifyPin(m_TestCard, "9999"));

    // after PIN verification failure, subsequent tasks such as getAccounts throw exceptions
    ASSERT_THROW(m_pAtmController->getAccounts(), std::runtime_error);
}

// Scenario 3: Withdraw more than in deposit
TEST_F(ATMControllerTest, WithdrawInsufficientFunds)
{
    using ::testing::Return;

    // Prerequisite setup for the test (PIN verificationa nd account selection)
    EXPECT_CALL(*m_pMockBankService, verifyPin(m_TestCard, "1234")).WillOnce(Return(true));
    m_pAtmController->insertCardAndVerifyPin(m_TestCard, "1234");
    m_pAtmController->selectAccount(m_TestAccount1); // balance is 100

    // Expect the bak service to return false when attempting to withdraw 200,
    // which is more than the balance.
    EXPECT_CALL(*m_pMockBankService, withdraw("ACCT-001", 200)).WillOnce(Return(false));

    // The result of the controller's withdraw method should be false.
    ASSERT_FALSE(m_pAtmController->withdraw(200));
}
