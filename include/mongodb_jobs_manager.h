/*
** Copyright 2014-2016 The Earlham Institute
** 
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
** 
**     http://www.apache.org/licenses/LICENSE-2.0
** 
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

/**
 * @file
 * @brief
 */
/*
 * apr_jobs_manager.h
 *
 *  Created on: 19 May 2015
 *      Author: tyrrells
 */

#ifndef APR_JOBS_MANAGER_H_
#define APR_JOBS_MANAGER_H_


#include "mongodb_jobs_manager_library.h"
#include "jobs_manager.h"
#include "mongodb_tool.h"


/*
 * The following preprocessor macros allow us to declare
 * and define the variables in the same place. By default,
 * they will expand to
 *
 * 		extern const char *SERVICE_NAME_S;
 *
 * however if ALLOCATE_JSON_TAGS is defined then it will
 * become
 *
 * 		const char *SERVICE_NAME_S = "path";
 *
 * ALLOCATE_RESOURCE_TAGS must be defined only once prior to
 * including this header file. Currently this happens in
 * resource.c.
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#ifdef ALLOCATE_APR_JOBS_MANAGER_TAGS
	#define APR_JOBS_MANAGER_PREFIX
	#define APR_JOBS_MANAGER_VAL(x)	= x
#else
	#define APR_JOBS_MANAGER_PREFIX extern
	#define APR_JOBS_MANAGER_VAL(x)
#endif

#endif 		/* #ifndef DOXYGEN_SHOULD_SKIP_THIS */



/**
 * This datatype is used by MongoDB
 * to be the Grassroots JobsManager.
 *
 * @extends JobsManager
 *
 */
typedef struct MongoDBJobsManager
{
	/** The base JobsManager */
	JobsManager mjm_base_manager;

	/**
	 * The MongoTool to connect to the
	 * MongoDB collection we will use.
	 */
	MongoTool *mjm_mongo_p;
} MongoDBJobsManager;




#ifdef __cplusplus
extern "C"
{
#endif


MONGODB_JOBS_MANAGER_API JobsManager *GetCustomJobsManager (void);


MONGODB_JOBS_MANAGER_API void ReleaseJobsManager (JobsManager *manager_p);


/**
 * @brief Get the JobsManager to use in the Grassroots module.
 *
 * @memberof MongoDBJobsManager
 * @return A newly-allocated MongoDBJobsManager or <code>NULL</code> upon error.
 */
MONGODB_JOBS_MANAGER_LOCAL MongoDBJobsManager *AllocateMongoDBJobsManager (void);


/**
 * @brief Free a MongoDBJobsManager.
 *
 * @param manager_p The MongoDBJobsManager to free.
 * @memberof MongoDBJobsManager
 */
MONGODB_JOBS_MANAGER_LOCAL bool FreeMongoDBJobsManager (JobsManager *manager_p);


#ifdef __cplusplus
}
#endif

#endif /* APR_JOBS_MANAGER_H_ */