FROM centos:centos7

# Dependencies
ENV INST yum install -y --setopt=skip_missing_names_on_install=False
RUN $INST https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm centos-release-scl
RUN $INST autoconf devtoolset-11 kernel-headers perl-core python36 python36-pip bzip2

# Environment Configuration
ENV CC=/opt/rh/devtoolset-11/root/usr/bin/gcc \
    CPP=/opt/rh/devtoolset-11/root/usr/bin/cpp \
    CXX=/opt/rh/devtoolset-11/root/usr/bin/g++ \
    PATH=/opt/rh/devtoolset-11/root/usr/bin:$PATH \
    LD_LIBRARY_PATH=/opt/rh/devtoolset-11/root/usr/lib64:/opt/rh/devtoolset-11/root/usr/lib:/opt/rh/devtoolset-11/root/usr/lib64/dyninst:/opt/rh/devtoolset-11/root/usr/lib/dyninst:/opt/rh/devtoolset-11/root/usr/lib64:/opt/rh/devtoolset-11/root/usr/lib:$LD_LIBRARY_PATH

## Python
RUN python3 -m pip install --upgrade pip wheel
RUN python3 -m pip install cmake

## Node
RUN curl -L https://raw.githubusercontent.com/tj/n/master/bin/n > /usr/bin/n
RUN chmod +x /usr/bin/n
RUN n 14
RUN npm i -g yarn

## LibUSB
RUN yum install -y libudev-devel

##
WORKDIR /nudelta
COPY . .
RUN yarn
RUN yarn build
RUN cp -r /nudelta/dist/*.AppImage /nudelta.AppImage
