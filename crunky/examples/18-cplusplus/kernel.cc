#include <mini_uart.h>
#include <stdio.h>


class Foo {
	public:
		Foo();
		~Foo();
		void hi();
	private:
		int meaning = 42;
};


Foo::Foo()
{
	puts("Foo created");
}

Foo::~Foo()
{
	puts("Foo being destroyed");
}

void Foo::hi()
{
	printf("foo says hi, and notes that the meaning of life is %d\n", meaning);
}


void test_foo()
{
	Foo foo;
	foo.hi();
}


void test_throw()
{
#if 0	
	puts("test_throw:begin");
	try {
		throw 20;
		puts("this is never printed");
	} catch (int e) {
		printf("Caught exception number %d\n", e);
	}
	puts("test_throw:exiting");
#endif
}


extern "C" void kernel_main(void)
{
	//uart_init(9600);
	uart_init_as_stdio(115200);

	Foo foo();

	puts("C++ test");
	test_foo();
	test_throw();

	puts("I will now echo what you type");
	while (1) {
		char c = getchar();
		putchar(c);
		//if(c=='\r') uart_send('\n'); // prolly needs linefeed
	}
}
