FROM ubuntu

RUN apt-get update \
    && apt-get install -y build-essential cmake libboost-all-dev \
                          wget git vim zsh libpthread-stubs0-dev \
    && apt-get clean
RUN sh -c "$(wget https://raw.githubusercontent.com/robbyrussell/oh-my-zsh/master/tools/install.sh -O -)"
RUN git clone https://github.com/joaovicentesouto/INE5418.git
RUN zsh /INE5418/install.sh

# CMD ping -w3 $BROTHER
# CMD echo ${BROTHER} >> shared_content/shared_object.txt && cat shared_content/shared_object.txt
CMD zsh ./INE5418/build/distributed_systems/distributed_systems_executable