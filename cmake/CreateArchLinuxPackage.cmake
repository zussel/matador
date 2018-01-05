MESSAGE(STATUS "Creating ArchLinux Package")

FIND_PROGRAM(TAR_PATH tar)
IF(NOT TAR_PATH)
  MESSAGE(FATAL_ERROR "tar not found! Aborting...")
ENDIF()

FIND_PROGRAM(XZ_PATH xz)
IF(NOT XZ_PATH)
  MESSAGE(FATAL_ERROR "xz not found! Aborting...")
ENDIF()

# create tmp/pkg dir in build dir
# mkdir -p tmp/pkg/usr/local
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tmp/pkg/usr/local)
file(MAKE_DIRECTORY ${SRC_DIR}/Package)

# create .PKGINFO file
# fill .PKGBUILD file with infos
# pkgname = matador
# pkgver = 0.5.0-1
# builddate = 1323390194
# pkgdesc = Matador - take your database by the horns
# url = https://zussel.github.io/matador
# packager = manual
# arch = x86_64
set(PKG_ROOT ${CMAKE_CURRENT_BINARY_DIR}/tmp/pkg)
MESSAGE(STATUS "PKG_ROOT: ${PKG_ROOT}")
string(TIMESTAMP builddate "%s" UTC)
file(WRITE ${PKG_ROOT}/.PKGINFO "pkgname = ${NAME}\n")
file(APPEND ${PKG_ROOT}/.PKGINFO "pkgver = ${VERSION}-1\n")
file(APPEND ${PKG_ROOT}/.PKGINFO "builddate = ${builddate}\n")
file(APPEND ${PKG_ROOT}/.PKGINFO "pkgdesc = ${DESC}\n")
file(APPEND ${PKG_ROOT}/.PKGINFO "url = ${URL}\n")
file(APPEND ${PKG_ROOT}/.PKGINFO "packager = manual\n")
file(APPEND ${PKG_ROOT}/.PKGINFO "arch = ${ARCH}\n")

# copy lib to tmp install dir
# cp -P -r $BUILD_ROOT/lib usr/local
file(COPY ${CMAKE_CURRENT_BINARY_DIR}/lib DESTINATION ${PKG_ROOT}/usr/local)

# cp include to tmp install dir
# cp -r $SRC_ROOT/include usr/local
file(COPY ${SRC_DIR}/include DESTINATION ${PKG_ROOT}/usr/local)

# correct version.hpp.in
# rm usr/local/include/matador/version.hpp.in
file(REMOVE ${PKG_ROOT}/usr/local/include/${VERSION_INCLUDE_DIR}/${VERSION_FILE}.in)
# cp $BUILD_ROOT/version.hpp usr/local/include/matador
file(COPY ${CMAKE_CURRENT_BINARY_DIR}/${VERSION_FILE} DESTINATION ${PKG_ROOT}/usr/local/include/${VERSION_INCLUDE_DIR}/)

# create archive
# tar -cf - .PKGINFO * | xz -c -z - > ../matador-0.5.0-1.pkg.tar.xz
# cp ../matador-0.5.0-1.pkg.tar.xz $SRC_ROOT/Package
# cp archive to Package dir
execute_process(COMMAND ${CMAKE_COMMAND} -E tar -cfJ ${SRC_DIR}/Package/${NAME}-${VERSION}-1-${ARCH}.pkg.tar.xz .PKGINFO usr
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tmp/pkg)

# delete tmp dir recursive
# rm -rf tmp
file(REMOVE_RECURSE ${CMAKE_CURRENT_BINARY_DIR}/tmp)

MESSAGE(STATUS "Archive was created: ${SRC_DIR}/Package/${NAME}-${VERSION}-1-${ARCH}.pkg.tar.xz")