doc_targets = []

xsltproc = find_program(
  'xsltproc',
  required: get_option('doc-man').enabled() or get_option('doc-pdf').enabled() or get_option('doc-html').enabled(),
)
fop = find_program('fop', required: get_option('doc-pdf'))

fo_stylesheet_url = 'http://docbook.sourceforge.net/release/xsl/current/fo/docbook.xsl'
man_stylesheet_url = 'http://docbook.sourceforge.net/release/xsl/current/manpages/docbook.xsl'
html_stylesheet_url = 'http://docbook.sourceforge.net/release/xsl/current/html/docbook.xsl'
html_chunk_stylesheet_url = 'http://docbook.sourceforge.net/release/xsl/current/html/chunk.xsl'

if xsltproc.found() and get_option('doc-man').enabled()
  ret = run_command(xsltproc, '--nonet', man_stylesheet_url, check: false)
  if ret.returncode() != 0
    error('Cannot build man pages without docbook-xsl-nons')
  endif
endif


# docbook is very spammy
run_quiet = find_program('run-quiet.py')

# .fncs files
doc_funcs_fncs = [
  'fcatomic',
  'fcblanks',
  'fccache',
  'fccharset',
  'fcconfig',
  'fcconstant',
  'fcdircache',
  'fcfile',
  'fcfontset',
  'fcformat',
  'fcfreetype',
  'fcinit',
  'fclangset',
  'fcmatrix',
  'fcobjectset',
  'fcobjecttype',
  'fcpattern',
  'fcrange',
  'fcstring',
  'fcstrset',
  'fcvalue',
  'fcweight',
]

fncs_files = []
foreach f : doc_funcs_fncs
  fncs_files += files('@0@.fncs'.format(f))
endforeach

man_pages = []

extract_man_list = find_program('extract-man-list.py')
man_list = run_command(extract_man_list, fncs_files, check: true).stdout().split()

foreach m : man_list
  man_pages += ['@0@.3'.format(m)]
endforeach

# Generate xml pages for funcs
edit_xml = find_program('edit-xml.py')

# copy into build directory, it includes generated files from build directory
fontconfig_devel_xml = configure_file(output: 'fontconfig-devel.xml',
  input: 'fontconfig-devel.xml',
  copy: true)

fontconfig_user_xml = configure_file(output: 'fontconfig-user.xml',
  input: 'fontconfig-user.xml',
  copy: true)

version_conf = configuration_data()
version_conf.set('VERSION', meson.project_version())

configure_file(output: 'version.xml',
  input: 'version.xml.in',
  configuration: version_conf)

confdir_conf = configuration_data()
confdir_conf.set('BASECONFIGDIR', fc_configdir)

confdir_xml = configure_file(output: 'confdir.xml',
  input: 'confdir.xml.in',
  configuration: confdir_conf)

funcs_xml = []

foreach f : doc_funcs_fncs
  funcs_xml += [custom_target('@0@.xml'.format(f),
    input: [files('func.xml'), files('@0@.fncs'.format(f))],
    output: '@0@.xml'.format(f),
    command: [edit_xml, '@INPUT0@', '@INPUT1@', '@OUTPUT@'],
    install: false)]
endforeach

if xsltproc.found()
  doc_targets += ['man']

  xsltproc_man = [
    xsltproc,
    '--output', '@OUTDIR@/',
    '--nonet',
    '--stringparam', 'man.authors.section.enabled', '0',
    '--stringparam', 'man.copyright.section.enabled', '0',
    '--stringparam', 'man.output.quietly', '1',
    # Do not insert middle header, it was also empty originally and XSL would put “[FIXME: manual]” there.
    '--stringparam', 'man.th.extra3.suppress', '1',
    # Do not insert middle footer, XSL would put date there, which would not be reproducible.
    '--stringparam', 'man.th.extra1.suppress', '1',
  ]

  custom_target('devel-man',
    input: [fontconfig_devel_xml, funcs_xml],
    output: man_pages,
    command: xsltproc_man + [
      man_stylesheet_url,
      '@INPUT0@',
    ],
    build_by_default: true,
    install_dir: get_option('mandir') / 'man3',
    install: true)

  # fonts.conf(5)
  custom_target('fonts-conf-5-man-page',
    input: [fontconfig_user_xml],
    output: 'fonts.conf.5',
    command: xsltproc_man + [
      # Do not insert left footer, it was also empty originally and XSL would put “[FIXME: source]” there.
      '--stringparam', 'man.th.extra2.suppress', '1',
      man_stylesheet_url,
      '@INPUT0@',
    ],
    install_dir: get_option('mandir') / 'man5',
    build_by_default: true,
    install: true)

  # Generate man pages for tools
  foreach t : tools_man_pages
    custom_target('@0@-man-page'.format(t),
      input: '../@0@/@0@.xml'.format(t),
      output: '@0@.1'.format(t),
      command: xsltproc_man + [
        # Do not insert left footer, it was also empty originally and XSL would put “[FIXME: source]” there.
        '--stringparam', 'man.th.extra2.suppress', '1',
        man_stylesheet_url,
        '@INPUT0@',
      ],
      install_dir: get_option('mandir') / 'man1',
      install: true)
  endforeach
endif

if xsltproc.found() and fop.found()
  doc_targets += ['PDF']

  devel_fo = custom_target(
    input: [fontconfig_devel_xml, funcs_xml],
    output: 'fontconfig-devel.fo',
    command: [xsltproc, '--output', '@OUTPUT@', fo_stylesheet_url, '@INPUT0@'],
  )

  custom_target(
    input: [devel_fo],
    output: 'fontconfig-devel.pdf',
    command: [fop, '-fo', '@INPUT@', '-pdf', '@OUTPUT@'],
    build_by_default: true,
    install_dir: get_option('datadir') / 'doc' / 'fontconfig',
    install: true,
  )

  user_fo = custom_target(
    input: [fontconfig_user_xml, funcs_xml],
    output: 'fontconfig-user.fo',
    command: [xsltproc, '--output', '@OUTPUT@', '--nonet', fo_stylesheet_url, '@INPUT0@'],
  )

  custom_target(
    input: [user_fo],
    output: 'fontconfig-user.pdf',
    command: [fop, '-fo', '@INPUT@', '-pdf', '@OUTPUT@'],
    build_by_default: true,
    install_dir: get_option('datadir') / 'doc' / 'fontconfig',
    install: true,
  )
endif

if xsltproc.found()
  doc_targets += ['HTML']

  custom_target('devel-html',
    input: [fontconfig_devel_xml, funcs_xml],
    output: 'fontconfig-devel',
    command: [
      xsltproc,
      '--output',
      '@OUTDIR@/fontconfig-devel/',
      '--nonet',
      '--stringparam', 'use.id.as.filename', '1',
      html_chunk_stylesheet_url,
      '@INPUT0@',
    ],
    build_by_default: true,
    install_dir: get_option('datadir') / 'doc' / 'fontconfig',
    install: true)

  custom_target('user-html',
    input: [fontconfig_user_xml, funcs_xml],
    output: 'fontconfig-user.html',
    command: [xsltproc, '--output', '@OUTPUT@', '--nonet', html_stylesheet_url, '@INPUT0@'],
    build_by_default: true,
    install_dir: get_option('datadir') / 'doc' / 'fontconfig',
    install: true)
endif
