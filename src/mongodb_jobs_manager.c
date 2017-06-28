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
/*
 * apr_jobs_manager.c
 *
 *  Created on: 18 May 2015
 *      Author: tyrrells
 */


#include "jobs_manager.h"

#include "mongodb_jobs_manager.h"

#include "service_job.h"
#include "string_utils.h"
#include "memory_allocations.h"
#include "grassroots_config.h"


#ifdef _DEBUG
#define MONGODB_JOBS_MANAGER_DEBUG	(STM_LEVEL_FINEST)
#else
#define MONGODB_JOBS_MANAGER_DEBUG	(STM_LEVEL_NONE)
#endif

/**
 * The MongoDBJobsManager stores key value pairs. The keys are the uuids for
 * the ServiceJobs that are converted to strings. The values are the ServiceJob
 * pointers.
 */

static bool AddServiceJobToMongoDBJobsManager (JobsManager *jobs_manager_p, uuid_t job_key, ServiceJob  *job_p);


static ServiceJob *QueryServiceJobFromMongoDBJobsManager (JobsManager *jobs_manager_p, const uuid_t job_key, bool get_job_flag, bool remove_job_flag);


static ServiceJob *GetServiceJobFromMongoDBJobsManager (JobsManager *manager_p, const uuid_t job_key);


static ServiceJob *RemoveServiceJobFromMongoDBJobsManager (JobsManager *manager_p, const uuid_t job_key, bool get_job_flag);


static LinkedList *GetAllServiceJobsFromMongoDBJobsManager (struct JobsManager *manager_p);


/**************************/


JobsManager *GetCustomJobsManager (void)
{
	MongoDBJobsManager *manager_p = AllocateMongoDBJobsManager ();

	if (manager_p)
		{
			return (& (manager_p -> mjm_base_manager));
		}

	return NULL;
}


void ReleaseJobsManager (JobsManager *manager_p)
{
	FreeMongoDBJobsManager (manager_p);
}



MongoDBJobsManager *AllocateMongoDBJobsManager (void)
{
	MongoTool *tool_p = AllocateMongoTool ();

	if (tool_p)
		{
			const char *database_s = "grassroots";
			const char *collection_s = "jobs";
			const json_t *config_p  = GetGlobalConfigValue ("mongodb_jobs_manager");

			if (config_p)
				{
					const char *value_s = GetJSONString (config_p, "database");

					if (value_s)
						{
							database_s = value_s;
						}

					value_s = GetJSONString (config_p, "collection");

					if (value_s)
						{
							collection_s = value_s;
						}
				}

			if (SetMongoToolCollection (tool_p, database_s, collection_s))
				{
					MongoDBJobsManager *manager_p = (MongoDBJobsManager *) AllocMemory (sizeof (MongoDBJobsManager));

					if (manager_p)
						{
							manager_p -> mjm_mongo_p = tool_p;

							InitJobsManager (& (manager_p -> mjm_base_manager), AddServiceJobToMongoDBJobsManager, GetServiceJobFromMongoDBJobsManager, RemoveServiceJobFromMongoDBJobsManager, GetAllServiceJobsFromMongoDBJobsManager, FreeMongoDBJobsManager);

							return manager_p;
						}
					else
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate memory for MongoDBJobsManager");
						}

				}		/* if (SetMongoToolCollection (tool_p, database_s, collection_s)) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to SetMongoToolCollection to database \"%s\" collection \"%s\"", database_s, collection_s);
				}

			FreeMongoTool (tool_p);
		}		/* if (tool_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to allocate memory for MongoDBJobsManager MongoTool");
		}


	return NULL;
}



bool FreeMongoDBJobsManager (JobsManager *manager_p)
{
	MongoDBJobsManager *mongodb_manager_p = (MongoDBJobsManager *) manager_p;

	FreeMongoTool (mongodb_manager_p -> mjm_mongo_p);
	FreeMemory (mongodb_manager_p);

	return true;
}



static bool AddServiceJobToMongoDBJobsManager (JobsManager *jobs_manager_p, uuid_t job_key, ServiceJob  *job_p)
{
	MongoDBJobsManager *manager_p = (MongoDBJobsManager *) jobs_manager_p;
	bool success_flag = false;
	char uuid_s [UUID_STRING_BUFFER_SIZE];

	Service *service_p = GetServiceFromServiceJob (job_p);

	if (service_p)
		{
			json_t *job_json_p = NULL;

			ConvertUUIDToString (job_key, uuid_s);

			if (DoesServiceHaveCustomServiceJobSerialisation (service_p))
				{
					job_json_p = CreateSerialisedJSONForServiceJobFromService (service_p, job_p);

					if (!job_json_p)
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create custom serialised job for %s from %s", uuid_s, GetServiceName (service_p));
						}
				}
			else
				{
					/* We store the c-style string for the ServiceJob's json */
					job_json_p = GetServiceJobAsJSON (job_p);

					if (!job_json_p)
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create serialised job for %s from %s", uuid_s, GetServiceName (service_p));
						}
				}


			if (job_json_p)
				{
					const char *error_s = EasyInsertOrUpdateMongoData (manager_p -> mjm_mongo_p, job_json_p, JOB_UUID_S);

					if (error_s)
						{
							PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "AddServiceJobToMongoDBJobsManager failed: \"%s\" for %s from %s", error_s, uuid_s, GetServiceName (service_p));
						}
					else
						{
							success_flag = true;
						}

				}		/* if (job_json_p) */

		}		/* if (service_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to get Service from job with id %s", uuid_s);
		}

	return success_flag;
}


static ServiceJob *GetServiceJobFromMongoDBJobsManager (JobsManager *manager_p, const uuid_t job_key)
{
	return QueryServiceJobFromMongoDBJobsManager (manager_p, job_key, true, false);
}



static ServiceJob *RemoveServiceJobFromMongoDBJobsManager (JobsManager *manager_p, const uuid_t job_key, bool get_job_flag)
{
	return QueryServiceJobFromMongoDBJobsManager (manager_p, job_key, get_job_flag, true);
}


static ServiceJob *QueryServiceJobFromMongoDBJobsManager (JobsManager *jobs_manager_p, const uuid_t job_key, bool get_job_flag, bool remove_job_flag)
{
	MongoDBJobsManager *manager_p = (MongoDBJobsManager *) jobs_manager_p;
	MongoTool *mongo_tool_p = manager_p -> mjm_mongo_p;
	ServiceJob *job_p = NULL;
	char uuid_s [UUID_STRING_BUFFER_SIZE];
	json_t *query_p = json_object ();

	if (query_p)
		{
			ConvertUUIDToString (job_key, uuid_s);

			if (json_object_set_new (query_p, JOB_UUID_S, json_string (uuid_s)) == 0)
				{
					if (FindMatchingMongoDocumentsByJSON (mongo_tool_p, query_p, NULL))
						{
							if (get_job_flag)
								{
									json_t *docs_p = GetAllExistingMongoResultsAsJSON (mongo_tool_p);

									if (docs_p)
										{
											if (json_is_array (docs_p))
												{
													const size_t num_docs = json_array_size (docs_p);

													if (num_docs == 1)
														{
															json_t *job_json_p = json_array_get (docs_p, 0);

															job_p = CreateServiceJobFromJSON (job_json_p);
														}
													else
														{

														}
												}
											else if (json_is_object (docs_p))
												{
													job_p = CreateServiceJobFromJSON (docs_p);
												}
											else
												{
													PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, docs_p, "Not valid object for holding ServiceJob");
												}

										}		/* if (docs_p) */
									else
										{
											PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, query_p, "Could not find any matching ServiceJobs");
										}

								}		/* if (get_job_flag) */

							if (remove_job_flag)
								{
									RemoveMongoDocuments (mongo_tool_p, query_p, false);
								}

						}		/* if (FindMatchingMongoDocumentsByJSON (mongo_tool_p, query_p, NULL)) */
					else
						{
							PrintJSONToErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, query_p, "Failed to find ServiceJob matching the given query");
						}

				}		/* if (json_object_set_new (query_p, JOB_UUID_S, uuid_s) == 0) */
			else
				{
					PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to add \"%s\": \"%s\" to query for locating ServiceJob", JOB_UUID_S, uuid_s);
				}

			json_decref (query_p);
		}		/* if (query_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create query to locate ServiceJob");
		}




	return job_p;
}


static LinkedList *GetAllServiceJobsFromMongoDBJobsManager (struct JobsManager *manager_p)
{
	LinkedList *jobs_p = AllocateLinkedList (FreeServiceJobNode);

	if (jobs_p)
		{

		}		/* if (jobs_p) */
	else
		{
			PrintErrors (STM_LEVEL_SEVERE, __FILE__, __LINE__, "Failed to create list for all ServiceJobs");
		}

	return jobs_p;
}

