DESCRIPTION = "hello world sample program"

PR = "r0"

DEPENDS = "gd"

SRC_URI = " \
  file://axis206.c \
  file://axis206.h \
  file://main.c \
  file://blob_gd.c \
  file://blob_gd.h \
  file://depth.c \
  file://depth.h \
  file://log.c \
  file://log.h \
  file://tolerance.c \
  file://tolerance.h \
  file://target.c \
  file://target.h \
"

S = "${WORKDIR}"

do_compile () {
    ${CC} ${CFLAGS} ${LDFLAGS} -lm -lgd target.c tolerance.c axis206.c blob_gd.c depth.c log.c main.c -o blob
}

do_install () {
    install -d ${D}${bindir}/
    install -m 0755 ${S}/blob ${D}${bindir}/
}

FILES_${PN} = "${bindir}/blob"

