cur_inc = include_directories('.')

mknodes = executable(
    'mknodes',
    'mknodes.c',
    include_directories: [root_inc, cur_inc],
    native: true,
)

mksyntax = executable(
    'mksyntax',
    'mksyntax.c',
    include_directories: [root_inc, cur_inc],
    native: true,
)

curdir = meson.current_source_dir()
bldir = meson.current_build_dir()

mkbuiltins_gen = custom_target(
    'builtins-gen',
    output: ['builtins.h', 'builtins.c'],
    command: [curdir / 'mkbuiltins', curdir, bldir],
)

mknodes_gen = custom_target(
    'nodes-gen',
    output: ['nodes.h', 'nodes.c'],
    command: [
        mknodes,
        curdir / 'nodetypes',
        curdir / 'nodes.c.pat',
        bldir / 'nodes.h',
        bldir / 'nodes.c',
    ],
)

mksyntax_gen = custom_target(
    'syntax-gen',
    output: ['syntax.h', 'syntax.c'],
    command: [mksyntax, bldir / 'syntax.h', bldir / 'syntax.c'],
)

mktokens_gen = custom_target(
    'token-gen',
    output: 'token.h',
    command: [curdir / 'mktokens', bldir],
)

compat_lib = static_library(
    'compat',
    [
        'compat/mktemp.c',
        'compat/setmode.c',
        'compat/signames.c',
        'compat/strlfuncs.c',
    ],
    include_directories: [root_inc, cur_inc],
    install: false,
)

bltins_lib = static_library(
    'bltins',
    [
        'bltin/echo.c',
        'bltin/kill.c',
        'bltin/printf.c',
        'bltin/test.c',
        mkbuiltins_gen,
    ],
    c_args: ['-DSHELL'],
    include_directories: [root_inc, cur_inc],
    install: false,
    pic: true,
)

sh_flags = []
sh_deps = []

if not libedit.found()
    sh_flags += ['-DNO_HISTORY']
else
    sh_deps += [libedit]
    if cc.has_function('fn_complete2', dependencies: libedit)
        sh_flags += ['-DHAVE_FN_COMPLETE2']
    endif
endif

if cc.has_argument('-Wclobbered')
    sh_flags += ['-Wno-clobbered']
endif

executable(
    'sh',
    [
        'alias.c',
        'arith_yacc.c',
        'arith_yylex.c',
        'cd.c',
        'error.c',
        'eval.c',
        'exec.c',
        'expand.c',
        'histedit.c',
        'input.c',
        'jobs.c',
        'mail.c',
        'main.c',
        'memalloc.c',
        'miscbltin.c',
        'mystring.c',
        'options.c',
        'output.c',
        'parser.c',
        'redir.c',
        'show.c',
        'trap.c',
        'var.c',
        mkbuiltins_gen,
        mknodes_gen,
        mksyntax_gen,
        mktokens_gen,
    ],
    c_args: sh_flags,
    include_directories: [root_inc, cur_inc],
    dependencies: sh_deps,
    link_with: [bltins_lib, compat_lib],
    install: true,
)

install_man('sh.1')
