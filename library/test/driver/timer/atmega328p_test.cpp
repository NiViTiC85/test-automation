/**
 * @brief Unit tests for the ATmega328p timer driver.
 */
#include <cstdint>

#include <gtest/gtest.h>

#include "arch/avr/hw_platform.h"
#include "driver/timer/atmega328p.h"
#include "utils/utils.h"

#ifdef TESTSUITE



//! @todo Implement tests according to project requirements.
namespace driver
{
namespace
{
/** Maximum number of timer circuits available on ATmega328P. */
constexpr std::uint8_t MaxTimerCount{3U};

/** Flag to track if callback was invoked. */
bool callbackInvoked{false};

// -----------------------------------------------------------------------------
void resetCallbackFlag() noexcept { callbackInvoked = false; }

// -----------------------------------------------------------------------------
void testCallback() noexcept { callbackInvoked = true; }

// -----------------------------------------------------------------------------
constexpr std::uint32_t getMaxCount(const std::uint32_t timeout_ms) noexcept
{
    // Calculate the number of times we have to invoke handleTimeout to reach a timeout.
    constexpr double interruptIntervalMs{0.128};
	return 0U < timeout_ms ? 
        utils::round<std::uint32_t>(timeout_ms / interruptIntervalMs) : 0U;
}



/**
 * @brief Timer initialization test.
 * 
 *        Verify that timer circuits are initialized correctly and that 
 *        resource limits are enforced.
 */
TEST(Timer_Atmega328p, Initialization)
{
    // Case 1 - Verify that only MaxTimerCount (3) timers can be used simultaneously due to 
    //          hardware limitations.
    {
        //! @todo Test timer initialization:
        //Create three timers.
        timer::Atmega328p timer0{100U};
        timer::Atmega328p timer1{50U};
        timer::Atmega328p timer2{25U};
        timer::Atmega328p timer3{200U};

        EXPECT_TRUE(timer0.isInitialized());
        EXPECT_TRUE(timer1.isInitialized());
        EXPECT_TRUE(timer2.isInitialized());
        EXPECT_FALSE(timer3.isInitialized());

            // Create MaxTimerCount timers with different timeouts.
            // Verify that each timer is initialized.
            // Create one additional timer.
            // Verify that the additional timer isn't initialized, since no circuits are available.
    }

    // Case 2 - Verify that a timer cannot have a 0 ms timeout.
    {
        // Create a timer with a 100 ms timeout.
        // Verify that the timer is initialized.
        timer::Atmega328p timer0{100U};
        EXPECT_TRUE(timer0.isInitialized());

        //  Create a timer with a 0 ms timeout.
        // Verify that the timer isn't initialized (0 ms is an invalid timeout).
        timer::Atmega328p timer1{0U};
        EXPECT_FALSE(timer1.isInitialized());
    }
}

/**
 * @brief Timer enable/disable test.
 * 
 *        Verify that timers can be started and stopped correctly.
 */
TEST(Timer_Atmega328p, EnableDisable)
{
    //! @todo Test timer enablement.
        // Create a timer with a timeout.
        timer::Atmega328p timer0(20U);
        // Verify timer is not enabled initially (unless auto-started via the constructor).
        EXPECT_FALSE(timer0.isEnabled());
        // Start the timer.
        //timer.start(timer0);
        timer0.start();
        // Verify that the timer is enabled.
        EXPECT_TRUE(timer0.isEnabled());
        // Stop the timer.
        //timer.stop(timer0);
        timer0.stop();
        // Verify that the timer is disabled.
        EXPECT_FALSE(timer0.isEnabled());
        // Toggle the timer.
        timer0.toggle();
        // Verify that the timer is enabled.
        EXPECT_TRUE(timer0.isEnabled());
        // Toggle the timer once again.
        timer0.toggle();
        // Verify that the timer is disabled.
        EXPECT_FALSE(timer0.isEnabled());

    //! @note Once the above is working:
    //!       Feel free to try all three timers. When enabling/disabling, feel free to check both
    //!       that the isEnabled() methods returns the right value and that the associated bit
    //!       in the timer mask register is set (see the source code).
    //!       Feel free to add a function and pass the timer, the mask register and the mask bit
    //!       to avoid writing the same code three times (or use a struct as was the case for
    //!       the registers in the GPIO unit tests).
}

/**
 * @brief Timer timeout test.
 * 
 *        Verify that timeout values can be set and read correctly.
 */
TEST(Timer_Atmega328p, Timeout)
{
    constexpr std::uint16_t timeout1{100U};
    constexpr std::uint16_t timeout2{200U};
    constexpr std::uint16_t timeout3{0U};

    //! @todo Test timer timeout:
        // Create a timer with an initial timeout of 100 ms.
        timer::Atmega328p timer0(timeout1);

        // Verify timeout_ms() returns the correct value.
        EXPECT_EQ(timer0.timeout_ms(), timeout1);

        // Change the timeout to 200 ms using setTimeout_ms().
        timer0.setTimeout_ms(timeout2);

        // Verify the new timeout is returned by timeout_ms().
        EXPECT_EQ(timer0.timeout_ms(), timeout2);

        // Change the timeout to 0 ms using setTimeout_ms().
        timer0.setTimeout_ms(timeout3);

        // Verify that the timeout is unchanged (0 ms is an invalid timeout).
        EXPECT_EQ(timer0.timeout_ms(), timeout2);
}

/**
 * @brief Timer callback test.
 * 
 *        Verify that timer callbacks are invoked when timeout occurs.
 */
TEST(Timer_Atmega328p, Callback)
{
    constexpr std::uint16_t timeout1{10U};

    // Reset the callback flag (callbackInvoked) using resetCallbackFlag().
    resetCallbackFlag();

    // Create a timer with a short timeout, such as 10 ms, and testCallback() as callback.
    timer::Atmega328p timer0(timeout1, testCallback);

    // Start the timer.
    timer0.start();

    // Calculate the number of times we have to call handleCallback to reach timeout.
    constexpr std::uint32_t maxCount{getMaxCount(timeout1)};
    
    // Simulate timer interrupts by repeatedly calling handleCallback() on the timer.
    // Call handleCallback() enough times to reach the timeout (getMaxCount()).
    for ( std::uint32_t i = 0 ; i < maxCount ; ++i )
    {
        timer0.handleCallback();
    }
    
    // Verify that callbackInvoked is true after timeout.
    EXPECT_TRUE (callbackInvoked);

    // Note: handleCallback() increments the timer and invokes the callback when timeout is reached.
}

/**
 * @brief Timer restart test.
 * 
 *        Verify that timers can be restarted correctly.
 */
TEST(Timer_Atmega328p, Restart)
{
    constexpr std::uint16_t timeout0{10U};

    //! @todo Test timer restart:
        // Reset the callback flag (callbackInvoked) using resetCallbackFlag().
        resetCallbackFlag();

        // Create and start a timer with testCallback() as callback.
        timer::Atmega328p timer0(timeout0, testCallback);
        timer0.start();

        // Call handleCallback() enough times to almost reach the timeout (getMaxCount() - 1).
        constexpr std::uint32_t maxCount0{getMaxCount(timeout0 - 1)};
        for ( std::uint32_t i = 0 ; i < maxCount0 ; ++i )
        {
            timer0.handleCallback();
        }

        // Verify that the callback flag (callbackInvoked) is still false.
        EXPECT_FALSE (callbackInvoked);

        // Restart the timer.
        timer0.restart();

        // Verify that the timer is still enabled after restart.
        EXPECT_TRUE (timer0.isEnabled());

        // Call handleCallback() enough times to almost reach the timeout (getMaxCount() - 1).
        for ( std::uint32_t i = 0 ; i < maxCount0 ; ++i )
        {
            timer0.handleCallback();
        }

        // Verify that the callback flag (callbackInvoked) is still false, since the timer was restarted.
        EXPECT_FALSE (callbackInvoked);

        // Call handleCallback() again to reach timeout.
        constexpr std::uint32_t maxCount1{getMaxCount(timeout0)};
        for ( std::uint32_t i = 0 ; i < maxCount1 ; ++i )
        {
            timer0.handleCallback();
        }

        // Verify that the callback flag (callbackInvoked) is true due to timeout.
        EXPECT_TRUE (callbackInvoked);

}

//! @todo Add more tests here (e.g., register verification, multiple timers running simultaneously).

} // namespace
} // namespace driver



#endif /** TESTSUITE */
