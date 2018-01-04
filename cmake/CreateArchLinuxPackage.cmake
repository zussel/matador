FUNCTION(CREATE_ARCHLINUX_PACKAGE)
  # create tmp/pkg dir in build dir
  # mkdir -p tmp/pkg
  # cd pkg

  # create .PKGINFO file
  # touch .PKGINFO

  # fill .PKGBUILD file with infos
  # pkgname = matador
  # pkgver = 0.5.0-1
  # builddate = 1323390194
  # pkgdesc = Matador - take your database by the horns
  # url = https://zussel.github.io/matador
  # packager = manual
  # arch = x86_64

  # create install dir (/usr/local) under tmp
  # mkdir -p usr/local

  # copy lib to tmp install dir
  # cp -P -r $BUILD_ROOT/lib usr/local

  # cp include to tmp install dir
  # cp -r $SRC_ROOT/include usr/local

  # correct version.hpp.in
  # rm usr/local/include/matador/version.hpp.in
  # cp $BUILD_ROOT/version.hpp usr/local/include/matador

  # create archive
  # tar -cf - .PKGINFO * | xz -c -z - > ../matador-0.5.0-1.pkg.tar.xz

  # cp archive to Package dir
  # cp ../matador-0.5.0-1.pkg.tar.xz $SRC_ROOT/Package

  # delete tmp dir recursive
  # rm -rf tmp
ENDFUNCTION()
