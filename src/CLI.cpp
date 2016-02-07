#include <CLI.h>

using namespace std;

duk_context *ctx;

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}
std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

void cliParse()
{
  printf("> ");
  string input;
  getline(std::cin, input);
  if (input == "exit")
  {
    cout << "Bye!\n";
    CleanupAndExit();
  }
  else if(input == "clear")
  {
    printf("\e[1;1H\e[2J");
  }
  else
  {
    duk_push_string(ctx, input.c_str());
    if (duk_peval(ctx) != 0) {
      printf("eval failed: %s\n", duk_safe_to_string(ctx, -1));
    } else {
      if (!strcmp(duk_safe_to_string(ctx, -1), "undefined") == 0)
      {
        printf("result is: %s\n", duk_safe_to_string(ctx, -1));
      }
    }
    duk_pop(ctx);  /* pop result */
  }
}

void *cliInit()
{
  printf("ProfileCAD CLI!\n");
  while(1)
  {
    cliParse();
  }
}

/***** Javascript functions **************/
int adder(duk_context *ctx) {
	int i;
	int n = duk_get_top(ctx);  /* #args */
	double res = 0.0;

	for (i = 0; i < n; i++) {
		res += duk_to_number(ctx, i);
	}

	duk_push_number(ctx, res);
	return 1;  /* one return value */
}
int DrawLine2D(duk_context *ctx) {

  line_t line;
	int i;
	int n = duk_get_top(ctx);  /* #args */
	double res = 0.0;

	/*for (i = 0; i < n; i++) {
		res += duk_to_number(ctx, i);
	}*/

  line.start.x = duk_to_number(ctx, 0);
  line.start.y = duk_to_number(ctx, 1);
  line.end.x = duk_to_number(ctx, 2);
  line.end.y = duk_to_number(ctx, 3);

  //debugDumpLineStructure(line);
  sceneColor(GREEN);
  cadDrawLine(line);
  PostRedisplay();

	duk_push_number(ctx, res);
	return 1;  /* one return value */
}


void cliRegisterFunctions()
{
  ctx = duk_create_heap_default();

  duk_push_global_object(ctx);
	duk_push_c_function(ctx, adder, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "adder");
	duk_pop(ctx);  /* pop global */

  duk_push_global_object(ctx);
	duk_push_c_function(ctx, DrawLine2D, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "NativeDrawLine2D");
	duk_pop(ctx);  /* pop global */

}

void cliRunScript(string file)
{
  if (duk_peval_file(ctx, file.c_str()) != 0)
  {
        printf("Error: %s\n", duk_safe_to_string(ctx, -1));
  }
  duk_pop(ctx);  /* ignore result */

  duk_push_string(ctx, "main();");
  if (duk_peval(ctx) != 0) {
    printf("eval failed: %s\n", duk_safe_to_string(ctx, -1));
  } else {
    if (!strcmp(duk_safe_to_string(ctx, -1), "undefined") == 0)
    {
      printf("result is: %s\n", duk_safe_to_string(ctx, -1));
    }
  }
  duk_pop(ctx);  /* pop result */

}
