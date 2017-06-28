# MongoDB Jobs Manager


## Installation

To build this service, you need the [grassroots core](https://github.com/TGAC/grassroots-core) and [grassroots build config](https://github.com/TGAC/grassroots-build-config) installed and configured. 

The files to build the MongoDB Jobs Manager are in the ```build/<platform>``` directory. 

### Linux

If you enter this directory 

```cd build/linux```

you can then build the service by typing

```make all```

and then 

```make install```

to install the manager into the Grassroots system.


## Configuration

To use this JobsManager module on the Grassroots system, you need to add an entry to the global configuration file ```grassroots.config```:

~~~.json
"jobs_manager": "mongodb_jobs_manager",
~~~

which specifies that this module will be used.

The module can be configured by adding a section called ```mongodb_jobs_manager``` where you can override the default database (this defaults to *grassroots*) and collection (this defaults to *jobs*) names. *e.g.*

~~~.json
"mongodb_jobs_manager": {
    "database": "my_database",
	"collection": "my_collection"
}	
~~~	

would specify to use a database called *my_database* and a collection called *my_collection*.
