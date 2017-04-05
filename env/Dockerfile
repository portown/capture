FROM ubuntu:16.04

RUN apt-get update && apt-get install -y --no-install-recommends \
    g++-mingw-w64-x86-64 \
    gcc-mingw-w64-x86-64 \
    binutils-mingw-w64-x86-64 \
    libz-mingw-w64-dev \
    make \
    curl \
    bzip2 \
    gcc \
    build-essential \
    unzip \
 && rm -rf /var/lib/apt/lists/*

RUN mkdir -p /home/capture
WORKDIR /home/capture

ENV LIBPNG_VERSION 1.6.8
ENV LIBPNG_DOWNLOAD_URL https://sourceforge.net/projects/libpng/files/libpng16/older-releases/$LIBPNG_VERSION/libpng-$LIBPNG_VERSION.tar.gz/download
ENV LIBPNG_DOWNLOAD_SHA1 a6d0be6facada6b4f26c24ffb23eaa2da8df9bd9
ENV LIBPNG_ROOT /usr/local/x86_64-w64-mingw32

RUN curl -fsSLk "$LIBPNG_DOWNLOAD_URL" -o libpng.tar.gz \
 && echo "$LIBPNG_DOWNLOAD_SHA1  libpng.tar.gz" | sha1sum -c - \
 && tar -xzf libpng.tar.gz \
 && ( cd libpng-$LIBPNG_VERSION && ./configure -q --prefix=$LIBPNG_ROOT --host=x86_64-w64-mingw32 --disable-shared --enable-static && make install && cd .. ) \
 && rm -Rf libpng-$LIBPNG_VERSION/ libpng.tar.gz

ENV BOOST_VERSION 1.63.0
ENV BOOST_DOWNLOAD_SHA1 9f1dd4fa364a3e3156a77dc17aa562ef06404ff6
ENV BOOST_ROOT /usr/local/x86_64-w64-mingw32

RUN curl -fsSLk "https://sourceforge.net/projects/boost/files/boost/$BOOST_VERSION/boost_`echo $BOOST_VERSION | sed -e 's/\./_/g'`.tar.bz2/download" -o boost.tar.bz2 \
 && echo "$BOOST_DOWNLOAD_SHA1  boost.tar.bz2" | sha1sum -c - \
 && tar -xjf boost.tar.bz2 \
 && ( cd boost_`echo $BOOST_VERSION | sed -e 's/\./_/g'` \
    && echo "using gcc : mingw : x86_64-w64-mingw32-g++ ;" > user-config.jam \
    && ./bootstrap.sh --without-icu --prefix=$BOOST_ROOT --with-libraries=container,filesystem,system \
    && ./b2 --user-config=user-config.jam \
        --layout=system \
        toolset=gcc-mingw \
        target-os=windows \
        variant=release \
        link=static \
        runtime-link=shared \
        threading=multi \
        threadapi=win32 \
        abi=ms \
        architecture=x86 \
        binary-format=pe \
        address-model=64 \
        install \
    && cd .. ) \
 && rm -Rf boost_`echo $BOOST_VERSION | sed -e 's/\./_/g'`/ boost.tar.bz2

RUN curl -fsSLk https://github.com/ptal/expected/archive/master.zip -o boost_expected.zip \
 && unzip -q boost_expected.zip \
 && cp -R expected-master/include/boost $BOOST_ROOT/include \
 && rm -Rf expected-master/ boost_expected.zip

RUN apt-get purge -y \
    curl \
    bzip2 \
    gcc \
    build-essential \
    unzip \
 && apt-get autoremove -y && rm -rf /var/lib/apt/lists/*
