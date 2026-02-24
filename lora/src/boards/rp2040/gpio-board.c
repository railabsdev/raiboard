/*
 * Copyright (c) 2021 Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */

#include "hardware/gpio.h"

#include "gpio-board.h"

#define GPIO_IRQ_PIN_COUNT 32

static GpioIrqHandler* gpio_irq_handlers[GPIO_IRQ_PIN_COUNT];
static void* gpio_irq_contexts[GPIO_IRQ_PIN_COUNT];

static void gpio_irq_dispatcher(uint gpio, uint32_t events)
{
    if (gpio < GPIO_IRQ_PIN_COUNT && gpio_irq_handlers[gpio] != NULL)
    {
        gpio_irq_handlers[gpio](gpio_irq_contexts[gpio]);
    }
}

static uint32_t gpio_irq_mode_to_mask(IrqModes irqMode)
{
    switch (irqMode)
    {
    case IRQ_RISING_EDGE:
        return GPIO_IRQ_EDGE_RISE;
    case IRQ_FALLING_EDGE:
        return GPIO_IRQ_EDGE_FALL;
    case IRQ_RISING_FALLING_EDGE:
        return GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL;
    case NO_IRQ:
    default:
        return 0;
    }
}

void GpioMcuInit( Gpio_t *obj, PinNames pin, PinModes mode, PinConfigs config, PinTypes type, uint32_t value )
{
    obj->pin = pin;
    obj->Context = NULL;

    if( pin == NC )
    {
        return;
    }

    gpio_init(pin);

    if (mode == PIN_INPUT)
    {
        gpio_set_dir(pin, GPIO_IN);
    }
    else if (mode == PIN_OUTPUT)
    {
        gpio_set_dir(pin, GPIO_OUT);
    }

    if (config == PIN_PUSH_PULL)
    {
        if (type == PIN_NO_PULL)
        {
            gpio_disable_pulls(pin);
        }
        else if (type == PIN_PULL_UP)
        {
            gpio_pull_up(pin);
        }
        else if (type == PIN_PULL_DOWN)
        {
            gpio_pull_down(pin);
        }
    }

    if( mode == PIN_OUTPUT )
    {
        GpioMcuWrite( obj, value );
    }
}

void GpioMcuSetContext( Gpio_t *obj, void* context )
{
    obj->Context = context;
}

void GpioMcuWrite( Gpio_t *obj, uint32_t value )
{
    gpio_put(obj->pin, value);
}

void GpioMcuToggle( Gpio_t *obj )
{
    gpio_put(obj->pin, !gpio_get(obj->pin));
}

uint32_t GpioMcuRead( Gpio_t *obj )
{
    return gpio_get(obj->pin);
}

void GpioMcuSetInterrupt( Gpio_t *obj, IrqModes irqMode, IrqPriorities irqPriority, GpioIrqHandler *irqHandler )
{
    (void)irqPriority;

    if (obj->pin == NC)
    {
        return;
    }

    uint32_t event_mask = gpio_irq_mode_to_mask(irqMode);
    if (event_mask == 0)
    {
        return;
    }

    if (obj->pin < GPIO_IRQ_PIN_COUNT)
    {
        gpio_irq_handlers[obj->pin] = irqHandler;
        gpio_irq_contexts[obj->pin] = obj->Context;
    }

    gpio_set_irq_enabled_with_callback(obj->pin, event_mask, true, gpio_irq_dispatcher);
}

void GpioMcuRemoveInterrupt( Gpio_t *obj )
{
    if (obj->pin == NC)
    {
        return;
    }

    gpio_set_irq_enabled(obj->pin, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, false);
    if (obj->pin < GPIO_IRQ_PIN_COUNT)
    {
        gpio_irq_handlers[obj->pin] = NULL;
        gpio_irq_contexts[obj->pin] = NULL;
    }
}
