FROM gitlab.cs.pub.ro:5050/iocla/tema-1-2024

RUN cargo install hw_checker

COPY ./checker ${CHECKER_DATA_DIRECTORY}
COPY ./data.json ${CHECKER_DATA_DIRECTORY}
