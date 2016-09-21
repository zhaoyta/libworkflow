# libworkflow
Workflow Engine in C++ 

## Purpose

To explain this library usage, I'll use a simple example. 
Let's say, you're a small business, you have multiples data providers that daily store data to your database. 
Everyday, you're looking through your data cross matching multiples events and making sense of all of this.
Problem is, for automation, you need to have a "fixed" workflow. 
If for some reason you get unexpected data, you either would have to do that the old fashion way, or you'll have to develop new algorithm and upgrade your software to work with this data. Even if the alteration isn't that big, it still means, potential bug introduction, testing, and so on. 
This lib, which is probably more a framework than a lib, allows you to have some more freedom in how your workflow work.
You still will have some developpement to do to have it working, but, this developpement will only be of Business related code: how to access your data, how to process them. 
The lib will provide communication or binding as to how this code shall be executed. More over, it allows customization of your workflow per request. 
Thus, you meet unexpected data in your daily workflow, you may either alter your workflow to investigate this unexpected data, or configure it differently to have a better view of the problem. 
You might also use this library to store your datas, filtering it, transforming it with workflows, totally customizable at runtime. 

## Features

* Workflow execution engine
** Action Api: This is where your code base will mostly be. Define a contract the action shall meet and define runtime expected configuration.
** Ressource Management and Scheduling Api: If you're dealing with limited resources, you can either use provided request schedulers or implement your own. 
* TCP / HTTP / Internal Api interface: This allow easy triggering execution of workflow. 
* Time based request execution scheduling



## Why ?

This is actually the third time I developed this engine. I kind of get experimented at doing this, and had the job opportunity to do it once again. This is the kind of haunting project coming again and again for me and I intend to do it a last time, once and for all. 

## How ?

I intend to use C++ 11, with boost 1.55 do manage this. I won’t provide a database to persist data, the whole workflow shall be provided with the request. Yet, I’ll leave enough flexibility to allow some workflow to be loaded on start. 

Tests will be handled using google test framework. It might not be exhaustive testing, nor TDD ready, but should cover the lib enough, at least to begin with. 

The lib and test executables will be compiled and tested under Mac OS X 10, and probably some debian as well, Jessie or another, not yet quite decided. 
