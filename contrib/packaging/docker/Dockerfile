FROM alpine:3.20 AS build
RUN apk add --no-cache build-base zlib-dev
ARG VERSION
WORKDIR /src
COPY . .
RUN make clean && make VERSION="$VERSION"

FROM alpine:3.20
RUN apk add --no-cache zlib
COPY --from=build /src/dist/pulse /pulse
COPY --from=build /src/src/app/static /src/app/static
EXPOSE 8383
ENTRYPOINT ["./pulse"]