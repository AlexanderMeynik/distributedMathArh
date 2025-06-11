---
config:
look: classic
    layout: elk
    theme: base
---
flowchart TD
subgraph utilLib["utilLib"]
parallelUtils["parallelUtils"]
commonLib["commonLib"]
allUtils["allUtils"]
end
    subgraph computationalLib["computationalLib"]
math_core_lib["math_core_lib"]
network_shared_lib["network_shared_lib"]
end
    subgraph computationalNode["computationalNode"]
computationalNodeLib["computationalNodeLib"]
end
    subgraph client["client"]
plotLib["plotLib"]
end
    subgraph mainNode["mainNode"]
mainNodeLib["mainNodeLib"]
end
    commonLib --> Eigen["Eigen3::Eigen"] & jsoncpp["jsoncpp_lib"] & fmt["fmt::fmt"] & scn["scn::scn"]
commonLib -.-> Math["Boost::math"]
allUtils --> commonLib & parallelUtils
    math_core_lib --> allUtils & OpenMP["OpenMP::OpenMP_CXX"]
network_shared_lib --> allUtils & amqpcpp["amqpcpp"] & curl["CURL::libcurl"] & pqxx["pqxx"] & pq["pq"]
network_shared_lib -.-> Asio["Boost::asio"]
computationalNodeLib --> math_core_lib & Drogon["Drogon::Drogon"] & network_shared_lib
    plotLib --> allUtils & Matplot["Matplot++::matplot"] & QtCharts["Qt6::Charts"] & QtDataVis["Qt6::DataVisualization"] & QtWidgets["Qt6::Widgets"]
mainNodeLib --> math_core_lib & network_shared_lib & Drogon
    style client fill:#D6F7DF
    style computationalNode fill:#D6F7DF
    style utilLib fill:#D6F7DF
    style mainNode fill:#D6F7DF
    style computationalLib fill:#D6F7DF
