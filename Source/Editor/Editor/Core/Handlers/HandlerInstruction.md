What is a handler?


A handler has to:
Have a strict layout with no private accessors. Only one update place, recommended editor as of now and a friend class to editor

Be of the syntax of somethingHandler

Handlers should share the lifetime of the engine and should therefor have either
A:
	A handlers script that you explicityly add them to and init in there and that provides a update function.

B:
	Integrate with systems in some way? Tbd


