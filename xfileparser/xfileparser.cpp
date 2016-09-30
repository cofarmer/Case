
#pragma warning(disable: 4995)
#pragma warning(disable: 4996)
#include "stdafx.h"
#include <Windows.h>
#include <direct.h>
#include "iconv/iconv.h"

#include "../../Public/EmlHelper/EmlBuilder.h"

#include "lua.hpp"
#pragma comment(lib, "../debug/lualib533")


#define MAGIC_EMLBUILDER "noema.emlbuilder"
struct eml_handle {
	CEmlBuilder *eml;
};

static int eml_new(lua_State *L);
static int eml_clean(lua_State *L);
static int eml_set_subject(lua_State *L);
static int eml_set_from(lua_State *L);
static int eml_set_to(lua_State *L);
static int eml_set_cc(lua_State *L);
static int eml_set_bcc(lua_State *L);
static int eml_set_senton(lua_State *L);
static int eml_set_textbody(lua_State *L);
static int eml_generate(lua_State *L);
static struct eml_handle *to_eml_handle(lua_State *L, int idx);

static const struct luaL_Reg emlhelper_f[] = {
	{ "new", eml_new },
	{ NULL, NULL },
};

static const struct luaL_Reg emlhelper_m[] = {
	{ "set_subject",  eml_set_subject },
	{ "set_from",  eml_set_from },
	{ "set_to",  eml_set_to },
	{ "set_cc",  eml_set_cc },
	{ "set_bcc",  eml_set_bcc },
	{ "set_sentdate",  eml_set_senton },
	{ "set_textbody",  eml_set_textbody },
	{ "generate",  eml_generate },
	{ "__gc",  eml_clean },
	{ NULL,  NULL },
};

static int eml_new(lua_State *L)
{
	eml_handle *eml = NULL;

	eml = (struct eml_handle *)lua_newuserdata(L, sizeof(struct eml_handle));
	if (!eml)
	{
		return 0;
	}
	eml->eml = CEmlBuilder::NewInstance();
	if (eml->eml)
	{
		eml->eml->Initialize();
	}

	luaL_getmetatable(L, MAGIC_EMLBUILDER);
	lua_setmetatable(L, -2);
	return 1;
}

static int eml_clean(lua_State *L)
{
	struct eml_handle *eml = NULL;
	eml = (struct eml_handle *)to_eml_handle(L, 1);
	if (eml && eml->eml)
	{
		delete eml->eml;
		eml->eml = NULL;
	}

	return 0;
}

static int eml_set_subject(lua_State *L)
{
	const char *subject = NULL;
	struct eml_handle *eml = NULL;

	eml = (struct eml_handle *)to_eml_handle(L, 1);
	subject = lua_tostring(L, 2);
	if (eml && eml->eml && subject)
	{
		eml->eml->SetSubject(subject);
	}
	return 0;
}

static int eml_set_from(lua_State *L)
{
	const char *from = NULL;
	struct eml_handle *eml = NULL;

	eml = (struct eml_handle *)to_eml_handle(L, 1);
	from = lua_tostring(L, 2);
	if (eml && eml->eml && from)
	{
		eml->eml->SetFrom(from);
	}
	return 0;
}

static int eml_set_to(lua_State *L)
{
	const char *to = NULL;
	struct eml_handle *eml = NULL;

	eml = (struct eml_handle *)to_eml_handle(L, 1);
	to = lua_tostring(L, 2);
	if (eml && eml->eml && to)
	{
		eml->eml->SetTo(to);
	}
	return 0;
}

static int eml_set_cc(lua_State *L)
{
	const char *cc = NULL;
	struct eml_handle *eml = NULL;

	eml = (struct eml_handle *)to_eml_handle(L, 1);
	cc = lua_tostring(L, 2);
	if (eml && eml->eml && cc)
	{
		eml->eml->SetCC(cc);
	}
	return 0;
}

static int eml_set_bcc(lua_State *L)
{
	const char *bcc = NULL;
	struct eml_handle *eml = NULL;

	eml = (struct eml_handle *)to_eml_handle(L, 1);
	bcc = lua_tostring(L, 2);
	if (eml && eml->eml && bcc)
	{
		eml->eml->SetBCC(bcc);
	}
	return 0;
}

static int eml_set_senton(lua_State *L)
{
	const char *sentdate = NULL;
	struct eml_handle *eml = NULL;

	eml = (struct eml_handle *)to_eml_handle(L, 1);
	sentdate = lua_tostring(L, 2);
	if (eml && eml->eml && sentdate)
	{
		eml->eml->SetSendTime(sentdate);
	}
	return 0;
}

static int eml_set_textbody(lua_State *L)
{
	const char *textbody = NULL;
	struct eml_handle *eml = NULL;

	eml = (struct eml_handle *)to_eml_handle(L, 1);
	textbody = lua_tostring(L, 2);
	if (eml && eml->eml && textbody)
	{
		eml->eml->SetTextBody((char *)textbody);
	}
	return 0;
}

static int eml_generate(lua_State *L)
{
	BOOL save_ret = FALSE;
	const char *file_path = NULL;
	struct eml_handle *eml = NULL;
	
	eml = (struct eml_handle *)to_eml_handle(L, 1);
	file_path = lua_tostring(L, 2);
	if (eml && eml->eml)
	{
		save_ret = eml->eml->SaveToFile(file_path);
	}

	lua_pushinteger(L, save_ret);

	return 1;
}

static struct eml_handle *to_eml_handle(lua_State *L, int idx)
{
	struct eml_handle *eml = NULL;
	eml = (struct eml_handle *)luaL_checkudata(L, idx, MAGIC_EMLBUILDER);
	if (eml == NULL)
	{
		luaL_argerror(L, idx, "invalid eml_handle object");
	}

	if (eml->eml == NULL)
	{
		luaL_error(L, "attempt to use closed eml_handle object");
	}

	return eml;
}

static int luaopen_emlbuilder(lua_State *L)
{
	luaL_newmetatable(L, MAGIC_EMLBUILDER);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");

	luaL_setfuncs(L, emlhelper_m, 0);

	lua_pop(L, 1);
	luaL_newlib(L, emlhelper_f);
	return 1;
}


static int print_log(lua_State *L)
{
	const char *msg;

	msg = lua_tostring(L, 1);
	if (msg)
	{
		printf("%s\r\n", msg);
	}

	return 0;
}

static int utf82gbk(lua_State *L)
{
	iconv_t cd;
	char *pin;
	char *pout;
	size_t inbytesleft;
	size_t outbytesleft;
	size_t r;
	char *str = NULL;
	const char *p = lua_tostring(L, 1);

	if (!p)
	{
		return 0;
	}
	cd = iconv_open("GBK", "UTF-8");
	if (cd == (iconv_t)(-1))
	{
		return 0;
	}

	size_t str_len = (strlen(p) + 1024) * sizeof(char);
	str = (char *)malloc(str_len);
	if(!str) return 0;
	memset(str, '\0', str_len);

	pin = (char *)p;
	pout = str;
	inbytesleft = strlen(p);
	outbytesleft = str_len;
	r = iconv(cd, &pin, &inbytesleft, &pout, &outbytesleft);
	if (r != (size_t)(-1))
		r = iconv(cd, NULL, NULL, &pout, &outbytesleft);
	*pout = 0;
	lua_pushstring(L, str);
	free(str);
	iconv_close(cd);
	return 1;
}

int xfileparse(const char *file_path,
			   const char *store_path,
			   const char *engin_file)
{
	int				ret;
	int				lret;
	lua_State		*L;
	char			app_path[1024];

	ret = -1;
	lret = 0;
	L = NULL;
	memset(app_path, '\0', sizeof(app_path));

	if (!file_path || !store_path || !engin_file)
	{
		return -1;
	}

	do 
	{
		GetModuleFileNameA(NULL, app_path, 1024);
		app_path[strlen(app_path) - strlen(strrchr(app_path, '\\')) + 1] = L'\0';

		L = luaL_newstate();
		if (!L)
		{
			break;
		}

		luaL_openlibs(L);

		// function table begin
		lua_newtable(L);

		lua_pushcfunction(L, print_log);
		lua_setfield(L, -2, "print_log");

		lua_pushcfunction(L, utf82gbk);
		lua_setfield(L, -2, "utf82gbk");

		luaopen_emlbuilder(L);
		lua_setfield(L, -2, "emlhelper");

		lua_setglobal(L, "func");
		// function table end

		// config table begin
		lua_newtable(L);
		
		lua_pushstring(L, app_path);
		lua_setfield(L, -2, "app_path");

		lua_setglobal(L, "config");
		// config table end

		lret = luaL_dofile(L, engin_file);
		if (lret != 0)
		{
			printf("lua says: %s \n", lua_tostring(L, -1));
			break;
		}

		lua_getglobal(L, "start_parse");
		lua_pushstring(L, file_path);
		lua_pushstring(L, store_path);
		if(0 != lua_pcall(L, 2, 1, 0))
		{
			printf("lua says: %s \n", lua_tostring(L, -1));
			lua_pop(L, 1);
			break;
		}

		ret = (int)lua_tointeger(L, -1);
		lua_pop(L, 1);
		if( 0 != ret)
		{
			printf("parse file failed, %s, code %d \n", file_path, ret);
			break;
		}

		ret = 0;

	} while (0);

	if (L)
	{
		lua_close(L);
	}

	return ret;
}

int _tmain(int argc, _TCHAR* argv[])
{
	char app_path[1024];
	char lua_file[1024];

	memset(app_path, '\0', sizeof(app_path));
	memset(lua_file, '\0', sizeof(lua_file));

	GetModuleFileNameA(NULL, app_path, 1024);
	app_path[strlen(app_path) - strlen(strrchr(app_path, '\\')) + 1] = L'\0';
	strcpy(lua_file, app_path);
	strcat(lua_file, "script\\parser.lua");

	xfileparse("e:\\nanjing.xml", app_path, lua_file);

	system("pause");

	return 0;
}

