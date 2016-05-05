set (tamal_src
  tam_apply.c tam_environment.c tam_eval.c tam_garbage.c
  tam_main.c tam_module_lispcore.c tam_object.c
  tam_object_nil.c tam_object_pair.c tam_object_symbol.c tam_primitive.c
  tam_symbol_table.c tam_auxeval.c tam_error.c tam_func.c tam_hashfunc.c
  tam_input.tab.c tam_module.c  tam_object_bool.c tam_object_module.c
  tam_object_number.c tam_object_proc.c tam_object_undef.c tam_stack.c
)

set (tamal_hdr
  tamal.h tam_auxeval.h tam_error.h  tam_func.h tam_hashfunc.h  tam_module.h
  tam_object_bool.h tam_object_module.h tam_object_number.h tam_object_proc.h
  tam_object_undef.h  tam_stack.h tam_apply.h  tam_environment.h tam_eval.h
  tam_garbage.h tam_input.h tam_module_lispcore.h tam_object.h
  tam_object_nil.h tam_object_pair.h tam_object_symbol.h tam_primitive.h tam_symbol_table.h)

include_directories(.)

add_executable (tamal ${tamal_src} ${tam_hdr})
