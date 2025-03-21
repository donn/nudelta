FROM quay.io/pypa/manylinux_2_28_x86_64

# Dependencies
## Python
RUN python3 -m ensurepip
RUN python3 -m pip install --upgrade pip wheel
RUN python3 -m pip install cmake

## Node
RUN curl -L https://raw.githubusercontent.com/tj/n/master/bin/n > /usr/bin/n
RUN chmod +x /usr/bin/n
RUN n 20
RUN npm i -g yarn

## LibUSB
RUN yum install -y libudev-devel

##
WORKDIR /nudelta
COPY . .
RUN yarn
RUN yarn build
RUN cp -r /nudelta/dist/*.AppImage /nudelta.AppImage
