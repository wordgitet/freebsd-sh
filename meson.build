project(
    'freebsd-sh',
    'c',
    version: '0.1.0',
    default_options: [
        'c_std=c99',
        'warning_level=2',
        'buildtype=debugoptimized',
    ],
    license: 'BSD-3-Clause',
    meson_version: '>=0.61',
)

cc = meson.get_compiler('c')

add_project_arguments(['-D_GNU_SOURCE', '-D_FILE_OFFSET_BITS=64'], language: 'c')
add_project_arguments('-Wno-unused-result', language: 'c')

if cc.has_argument('-Wstring-plus-int')
    add_project_arguments('-Wno-string-plus-int', language: 'c')
endif

libedit = dependency('libedit', required: get_option('libedit'))

confd = configuration_data()
confd.set('HAVE_STRTOQ', cc.has_function('strtoq'))
confd.set('HAVE_STRLCPY', cc.has_function('strlcpy'))
confd.set('HAVE_STRLCAT', cc.has_function('strlcat'))
confd.set('HAVE_STRTONUM', cc.has_function('strtonum'))
strchrnul_args = []
if cc.has_argument('-Werror=unguarded-availability-new')
    strchrnul_args += ['-Werror=unguarded-availability-new']
endif

confd.set('HAVE_STRCHRNUL', cc.links('''
#include <string.h>
int main(void) {
    return !strchrnul("", 0);
}
''', name: 'strchrnul', args: strchrnul_args))
confd.set('HAVE_REALLOCF', cc.has_function('reallocf'))
confd.set('HAVE_REALLOCARRAY', cc.has_function('reallocarray'))
confd.set('HAVE_ERRC', cc.has_function('errc'))
confd.set('HAVE_WARNC', cc.has_function('warnc'))

configure_file(output: 'config-compat.h', configuration: confd)

root_inc = include_directories('.', 'include')

subdir('src')
