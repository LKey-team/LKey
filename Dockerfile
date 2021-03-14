FROM gcc:latest
COPY . .
RUN gcc -o myapp main.cpp