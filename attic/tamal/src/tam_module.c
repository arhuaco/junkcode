#include <tamal.h>
#include <tam_module.h>

static TAMModuleType *module_table[]
=
{
  &TAMModuleLispCore
};

TAMModuleType * tam_module_type_by_name_get (char *name)
{
  TAMModuleType     *module_type = NULL;
  int i;

  ASSERT(name);

  for (i = 0; i < sizeof(module_table)/sizeof(module_table[0]); ++i)
    {
      ASSERT(module_table[i]->name);

      if (!strcmp(module_table[i]->name, name))
        {
          module_type = module_table[i];
          break;
        }
    }

  return module_type;
}
