#pragma once

#define WGSEOPSIZE 2
#define WGSEOPTYPE unsigned short
#define WGSEOPMAX  (0x7FFF)

struct WGSEVM;
struct WGSEMODULE;
struct WGSETYPE;
struct WGSETYPEINSTANCE;

typedef size_t WGSESTACKELEMENT;

typedef void(*WGSEFUNCTION)(struct WGSEVM*);

#define WGSETYPE_

struct WGSETYPE
{
	char type;
};


/*
Scopes:
- Global
	- module
		- init functioin ?
		- module
			- constant
			- variable
		- class
			- this function
			- static function
			- static var
			- this var

Dir structure:
scripts/
		app.wgs - root application module, contains some constants, global vars, can contain classes and functions
		external.wgs - contains bindings to core application
		module1.wgs - module scope constants, variables and classes
		module1/
				class1.wgs - class interface, and all embeded functions
				class1/
						init0.wgs - non embeded function - constructor
						foobar.wgs - non embeded function
Data format:
- must be easly extendable - parser can skip unknown data
- must be Version Control friendly - isolate changes, new symbols add to end

JSON?
TXT?
OWN!


module "TestModule1"
	embedded 1
	objects
		variable "var"
			type float
			default 1.0
			limits (-100.0, 100.0)
		function "foobar" (in:exec, x:float, y:float) (out:exec, res:float)
			descr "function description"
			input
				x
					descr "argument description"
				y 
					descr "argument description"
			output
				res
					descr "argument description (test)"
			pure 0
			body
				node interface
					editor
						pos (10, 10)
						size (40, 20)
					links
						-> out
							link sum.in
				node sum
					linkage math.sum
					links
						<- arg0
							input.x
						<- arg1
							input.y
						-> out
							output.out
				node output
					links
						<- res
							link sum.result
*/