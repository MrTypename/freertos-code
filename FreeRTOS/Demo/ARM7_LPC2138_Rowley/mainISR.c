/*
    FreeRTOS V8.0.0:rc1 - Copyright (C) 2014 Real Time Engineers Ltd. 
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that has become a de facto standard.             *
     *                                                                       *
     *    Help yourself get started quickly and support the FreeRTOS         *
     *    project by purchasing a FreeRTOS tutorial book, reference          *
     *    manual, or both from: http://www.FreeRTOS.org/Documentation        *
     *                                                                       *
     *    Thank you!                                                         *
     *                                                                       *
    ***************************************************************************

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

    >>! NOTE: The modification to the GPL is included to allow you to distribute
    >>! a combined work that includes FreeRTOS without being obliged to provide
    >>! the source code for proprietary components outside of the FreeRTOS
    >>! kernel.

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available from the following
    link: http://www.freertos.org/a00114.html

    1 tab == 4 spaces!

    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?"                                     *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org - Documentation, books, training, latest versions,
    license and Real Time Engineers Ltd. contact details.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.OpenRTOS.com - Real Time Engineers ltd license FreeRTOS to High
    Integrity Systems to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/
#include "FreeRTOS.h"
#include "semphr.h"

#define isrCLEAR_EINT_1 2

/*
 * Interrupt routine that simply wakes vButtonHandlerTask on each interrupt 
 * generated by a push of the built in button.  The wrapper takes care of
 * the ISR entry.  This then calls the actual handler function to perform
 * the work.  This work should not be done in the wrapper itself unless
 * you are absolutely sure that no stack space is used.
 */
void vButtonISRWrapper( void ) __attribute__ ((naked));
void vButtonHandler( void ) __attribute__ ((noinline));

void vButtonHandler( void )
{
extern SemaphoreHandle_t xButtonSemaphore;
portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	xSemaphoreGiveFromISR( xButtonSemaphore, &xHigherPriorityTaskWoken );

	if( xHigherPriorityTaskWoken )
	{
		/* We have woken a task.  Calling "yield from ISR" here will ensure
		the interrupt returns to the woken task if it has a priority higher
		than the interrupted task. */
		portYIELD_FROM_ISR();
	}

    EXTINT = isrCLEAR_EINT_1;
    VICVectAddr = 0;
}
/*-----------------------------------------------------------*/

void vButtonISRWrapper( void )
{
	/* Save the context of the interrupted task. */
	portSAVE_CONTEXT();

	/* Call the handler to do the work.  This must be a separate function to
	the wrapper to ensure the correct stack frame is set up. */
	__asm volatile( "bl vButtonHandler" );

	/* Restore the context of whichever task is going to run once the interrupt
	completes. */
	portRESTORE_CONTEXT();
}



