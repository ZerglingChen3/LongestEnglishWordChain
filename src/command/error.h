#ifndef _ERROR_H_
#define _ERROR_H_

enum Error {
	FILE_NOT_FIND = 1,
	HAVE_LOOP_ERROR,
	MULTI_FILE_PATH,
	PARAMETER_NOT_EXISTS,
	NO_FILE_PATH,
	FILE_PATH_FORM_ERROR,
	NO_CHAR_ERROR,
	CHAR_FORM_ERROR,
	PARAMETER_FORM_ERROR,
	ALLOC_MEMORY_ERROR,
	MULTI_WORK_ERROR,
	FIRST_CHAR_DUPLICATE,
	FINAL_CHAR_DUPLICATE,
	ENABLE_LOOP_DUPLICATE,
	N_WORK_WITH_OTHER_PARAMETER,
	M_WORK_WITH_OTHER_PARAMETER
};

#endif