# libworkflow
Workflow Engine in C++ 

Why ?
--------

This is actually the third time I developed this engine. I kind of get experimented at doing this, and had the job opportunity to do it once again. This is the kind of haunting project coming again and again for me and I intend to do it a last time, once and for all. 

What ?
--------

My workflow engine, in opposition to most of those you may find, is aimed at doing things. Things that might come in a specific order in a context, and in a completely different one in another. Yet each block would be the same. 

Each block, called Action, are connected to one another through a series of inputs and outputs, exchanging data called Contexts. These Actions can be fine tuned by properties, and each time a Workflow is called, that is a directed tree of actions, a new set of property might be provided as well, ensuring the tuning you might need. 

Contexts on the other hand are hard data, that will change on each call. It’s the Data that is acquired during an action, or transformed by it or used by it, to do it’s bidding. 

Workflow belongs to a Controller, whose job is to ensure the workflow can be used right now. Thus it allows some workflow to be restricted of usage, think of limited ressources that can be used only one at a time, or fully accessible. The controllers job is to ensure these criterion are met before allow a Request to be executed. 

This Engine also provide interfaces with outside, default interface will be a TCP interface serving JSON, but it will be open enough for any other means to be implemented. 

Also, connecting several workflow engine should be permitted, allowing back and forth requests. This isn’t a stateless thing, yet it could become one if this is requested. 

How ?
-------

I intend to use C++ 11, with boost 1.55 do manage this. I won’t provide a database to persist data, the whole workflow shall be provided with the request. Yet, I’ll leave enough flexibility to allow some workflow to be loaded on start. 

Tests will be handled using google test framework. It might not be exhaustive testing, nor TDD ready, but should cover the lib enough, at least to begin with. 

The lib and test executables will be compiled and tested under Mac OS X 10, and probably some debian as well, Jessie or another, not yet quite decided. 

When ?
----------

The plan is to push every week new / updated code. Note that I still have a day time job ;) I might not be extraordinarily productive, but I intend to go through all this. 

I shall quickly produce some plan and wikis as to what shall be done first, and what is precisely to be expected soon. 

Who ?
-------

For the most part, I intend to do this alone, yet, all help shall be welcome, you can contact me here anytime. 

I am open to suggestion, commentary, thumbs up or down, or beer ;)
