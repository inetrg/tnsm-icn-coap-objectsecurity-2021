# Content Object Security in the Internet of Things: Challenges, Prospects, and Emerging Solutions

[![Paper][paper-badge]][paper-link]

This repository contains code and documentation to reproduce experimental results of the paper **[Content Object Security in the Internet of Things: Challenges, Prospects, and Emerging Solutions][paper-link]** published in the IEEE Transactions on Network and Service Management (TNSM).

* Cenk Gündogan, Christian Amsüss, Thomas C. Schmidt, Matthias Wählisch,
**Content Object Security in the Internet of Things: Challenges, Prospects, and Emerging Solutions**,
In: IEEE Transactions on Network and Service Management (TNSM), Vol. 19, No. 1, pp. 538--553, March 2022.

  **Abstract**
  > Content objects are confined data elements that carry meaningful information. Massive amounts of content objects are published and exchanged every day on the Internet.  The emerging Internet of Things (IoT) augments the network edge with reading sensors  and controlling actuators that comprise machine-to-machine communication using small data objects. IoT content objects are often messages that fit into single IPv6 datagram. These IoT messages frequently traverse protocol translators at gateways, which break end-to-end transport and security of Internet protocols.  To preserve content security from end to end via gateways and proxies, the IETF recently developed Object Security for Constrained RESTful Environments (OSCORE), which extends the Constrained Application Protocol (CoAP) with content object security features commonly known from Information Centric Networking (ICN).
  >
  > This paper revisits the current IoT protocol architectures and presents a comparative analysis of protocol stacks that protect request-response transactions. We discuss features and limitations of the different protocols and analyze emerging functional extensions.  We measure the protocol performances of CoAP over Datagram Transport Layer Security (DTLS), OSCORE, and the information-centric Named Data Networking (NDN) protocol on a large-scale IoT testbed in single- and multi-hop scenarios.  Our findings indicate that (a) OSCORE improves on CoAP over DTLS in error-prone wireless regimes due to omitting the  overhead of maintaining security sessions at endpoints,  (b) NDN attains superior robustness and reliability due to its intrinsic network caches and hop-wise retransmissions, and (c) OSCORE/CoAP offers room for improvement and optimization in multiple directions.

Please follow our [Getting Started](getting-started.md) instructions for further information on how to compile and execute the code.

[paper-link]:https://doi.org/10.1109/TNSM.2021.3099902
[paper-badge]:https://img.shields.io/badge/Paper-IEEE%20Xplore-green
