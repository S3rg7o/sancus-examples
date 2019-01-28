#include <msp430.h>
#include <stdio.h>
#include <sancus/sm_support.h>
#include <sancus_support/sm_io.h>
#include "dma.h"

void exit_success(void);

/* ======== HELLO WORLD SM ======== */

DECLARE_SM(hello, 0x1234);
#define HELLO_SECRET_CST    0xC1A0

int       SM_DATA(hello) *hello_secret;
int const SM_DATA(hello)  hello_const = HELLO_SECRET_CST;

void SM_FUNC(hello) hello_init(void)
{
    /* Confidential loading guarantees secrecy of constant in text section. */
    
    hello_secret = (int*)(hello.secret_start+7);
    *hello_secret = hello_const;
    ASSERT(*hello_secret == HELLO_SECRET_CST);
}

void SM_ENTRY(hello) hello_greet(void)
{
    hello_init();
    pr_info2("Hi from SM with ID %d, called by %d\n", sancus_get_self_id(), sancus_get_caller_id());
    pr_info2("Internally accessing to my secret: 0x%.4x at addr.: 0x%.4x \n",*hello_secret, hello_secret);	
}

void SM_ENTRY(hello) hello_disable(void)
{
    sancus_disable(exit_success);
}

/* ======== UNTRUSTED CONTEXT ======== */

volatile int *hello_const_pt = (volatile int *) &hello_const;

int main()
{
	uint16_t start_dma;
	uint16_t disclosed_secret;
	
    msp430_io_init();
    ASSERT((*hello_const_pt) != HELLO_SECRET_CST);

    sancus_enable_wrapped(&hello, SM_GET_WRAP_NONCE(hello), SM_GET_WRAP_TAG(hello));
    pr_sm_info(&hello);
	
    hello_greet();

	
	/* ======= USING DMA ======== */
	start_dma = (uint16_t)(hello.secret_start+7);
	puts("DMA illegal access to the hello_secret from unprotected code \n"); 
	dma_read(start_dma, 1, &disclosed_secret);
	pr_info2("Hello secret is: 0x%.4x at addr.: 0x%.4x \n", disclosed_secret, start_dma);
	/* ========================== */
		
	
	
    hello_disable();
    // should never reach here
    ASSERT(0);
}

void exit_success(void)
{
    // TODO unprotect instruction should also clear caller ID
    //ASSERT(!sancus_get_caller_id());
    ASSERT(!sancus_get_id(hello_greet));
    ASSERT(!hello_secret);
    ASSERT(!(*hello_const_pt));

    pr_info("SM disabled; all done!");
    EXIT();
}
