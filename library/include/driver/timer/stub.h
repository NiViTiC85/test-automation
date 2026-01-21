/**
 * @brief Timer interface.
 */
#pragma once

#include <stdint.h>

#include "driver/timer/interface.h"

namespace driver
{
namespace timer
{
/**
 * @brief Timer interface.
 */
class Stub final : public Interface
{
public:
    /**
     * @brief Contructor
     */
    Stub() noexcept
        : myInitialized{true}
        , myTimeout{false}
        , myEnabled{false}
    {}

    /**
     * @brief Destructor.
     */
    ~Stub() noexcept override = default;

    /**
     * @brief Check if the timer is initialized.
     * 
     *        An uninitialized timer indicates that no timer circuit was available when the timer 
     *        was created.
     * 
     * @return True if the timer is initialized, false otherwise.
     */
    bool isInitialized() const noexcept override { return myInitialized; }

    /**
     * @brief Check whether the timer is enabled.
     *
     * @return True if the timer is enabled, false otherwise.
     */
    bool isEnabled() const noexcept override { return myEnabled; }

    /**
     * @brief Check whether the timer has timed out.
     *
     * @return True if the timer has timed out, false otherwise.
     */
    bool hasTimedOut() const noexcept override { return myTimeout; }

    /**
     * @brief Get the timeout of the timer.
     * 
     * @return The timeout in milliseconds.
     */
    uint32_t timeout_ms() const noexcept override 
    { 
        // Timeout is irrelevant for the stub, always return 0.
        return 0;
    }

    /**
     * @brief Set timeout of the timer.
     * 
     * @param[in] timeout_ms The new timeout in milliseconds.
     */
    void setTimeout_ms(uint32_t timeout_ms) noexcept override
    {
        // Timeout is irrelevant for the stup, do nothing.
        (void) (timeout_ms);
    }

    /**
     * @brief Start the timer.
     */
    void start() noexcept override
    {
        if (myInitialized) { myEnabled = true; }
    }

    /**
     * @brief Stop the timer.
     */
    void stop() noexcept override
    {
        if (myInitialized) { myEnabled = false; }
    }

    /**
     * @brief Toggle the timer.
     */
    void toggle() noexcept override
    {
        if (myInitialized) { myEnabled = !myEnabled; }
    }

    /**
     * @brief Restart the timer.
     */
    void restart() noexcept override
    {
        if (myInitialized) 
        { 
            myEnabled = true;
            myTimeout = false; 
        }
    }

    /**
     * @brief Set timeout status.
     * 
     * @param[in] timedOut Indicate that the timer has timed out.
     * 
     */
    void setTimedOut(bool timedOut) noexcept 
    {
        // Set timeout status if the timer is initialized.
        if (myInitialized) { myTimeout = timedOut; }
    }

    Stub(const Stub&)            = delete; // No copy constructor.
    Stub(Stub&&)                 = delete; // No move constructor.
    Stub& operator=(const Stub&) = delete; // No copy assignment.
    Stub& operator=(Stub&&)      = delete; // No move assignment.

private: 
    bool myInitialized; 
    bool myTimeout;
    bool myEnabled;
};
} // namespace timer
} // namespace driver