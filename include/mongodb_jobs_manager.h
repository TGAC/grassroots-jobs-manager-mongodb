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

#ifndef MONGODB_JOBS_MANAGER_H_
#define MONGODB_JOBS_MANAGER_H_


#include "mongodb_jobs_manager_library.h"
#include "jobs_manager.h"
#include "mongodb_tool.h"



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

} MongoDBJobsManager;




#ifdef __cplusplus
extern "C"
{
#endif


/**
 * The callback function for allocating a MongoDBJobsManager.
 *
 * @return The newly-allocated MongoDBJobsManager
 * or <code>NULL</code> upon error.
 * @memberof MongoDBJobsManager
 */
MONGODB_JOBS_MANAGER_API JobsManager *GetCustomJobsManager (void);


/**
 * The callback function for freeing the MongoDBJobsManager.
 *
 * This will call FreeMongoDBJobsManager().
 * @param manager_p The MongoDBJobsManager to free.
 * @memberof MongoDBJobsManager
 */
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
 * @return <code>true</code> if the MongoDBJobsManager was freed successfully,
 * <code>false</code> otherwise
 * @memberof MongoDBJobsManager
 */
MONGODB_JOBS_MANAGER_LOCAL bool FreeMongoDBJobsManager (JobsManager *manager_p);


#ifdef __cplusplus
}
#endif

#endif /* MONGODB_JOBS_MANAGER_H_ */
