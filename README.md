# What is OceanBase Database
OceanBase Database is a native distributed relational database. It is developed entirely by Ant Group. OceanBase Database is built on a common server cluster. Based on the Paxos protocol and its distributed structure, OceanBase Database provides high availability and linear scalability. OceanBase Database is not dependent on specific hardware architectures.

## Core features

- Scalable OLTP
   - Linear scalability by adding nodes to the cluster
   - Partition-level leader distribution and transparent data shuffling 
   - Optimized performance for distributed transaction through "table group" technology
   - High concurrency updates on hot row through early lock release (ELR)
   - 80000+ connections per node and unlimited connections in one instance through multi threads and coroutines
   - Prevent silent data corruption (SDC) through multidimensional data consistency checksum
   - No.1 in TPC-C benchmark with 707 million tpmC
- Operational OLAP
   - Process analytical tasks in one engine, no need to migrate data to OLAP engine
   - Analyze large amounts of data on multiple nodes in one OceanBase cluster with MPP architecture
   - Advanced SQL engine with CBO optimizer, distributed execution scheduler and global index
   - Fast data loading through parallel DML, and with only 50% storage cost under compression
   - Broke world record with 15.26 million QphH in TPC-H 30TB benchmark in 2021
- Multi-tenant
   - Create multiple tenants (instances) in one OceanBase cluster with isolated resource and access
   - Multidimensional and transparently scale up/out for each tenant, and scaling up takes effect immediately
   - Database consolidation: multi-tenant and flexible scaling can achieve resource pooling and improve utilization
   - Improve management efficiency and reduce costs without compromising performance and availability

## Quick start
See [Quick start](https://open.oceanbase.com/quickStart) to try out OceanBase Database.

## System architecture

## ![image.png](https://cdn.nlark.com/yuque/0/2022/png/106206/1660643534074-2649c2e4-473a-4d07-8021-d8d1a2b2da49.png#clientId=u5d9acd3a-25ad-4&crop=0&crop=0&crop=1&crop=1&from=paste&height=1101&id=u46f6dd09&margin=%5Bobject%20Object%5D&name=image.png&originHeight=1101&originWidth=1746&originalType=binary&ratio=1&rotation=0&showTitle=false&size=130476&status=done&style=none&taskId=ud87a5d0f-5140-45f2-9274-c8375b3c7c0&title=&width=1746)

## Roadmap

![image.png](https://cdn.nlark.com/yuque/0/2022/png/106206/1660643534293-a5f53258-a9ac-462c-b9fd-9832901853c2.png#clientId=u5d9acd3a-25ad-4&crop=0&crop=0&crop=1&crop=1&from=paste&height=352&id=u0f71535e&margin=%5Bobject%20Object%5D&name=image.png&originHeight=528&originWidth=1683&originalType=binary&ratio=1&rotation=0&showTitle=false&size=719961&status=done&style=none&taskId=u676e97dd-1309-42b9-b380-a423c27199c&title=&width=1122)

Link: [3.1.5 function list](https://github.com/oceanbase/oceanbase/milestone/6)

## Case study
For our success stories, see [Success stories](https://www.oceanbase.com/en/customer/home).

## Contributing
Your contributions to our code will be highly appreciated. For details about how to contribute to OceanBase, see [Contribute to OceanBase](https://github.com/oceanbase/oceanbase/wiki/Contribute-to-OceanBase).

## Licensing
OceanBase Database is under [MulanPubL - 2.0](http://license.coscl.org.cn/MulanPubL-2.0/#english) license. You can freely copy and use the source code. When you modify or distribute the source code, please follow the MulanPubL - 2.0 license.

## Community

- [oceanbase.slack](https://oceanbase.slack.com/)
- [Forum (Simplified Chinese)](https://ask.oceanbase.com/)
- [DingTalk 33254054 (Simplified Chinese)](https://h5.dingtalk.com/circle/healthCheckin.html?corpId=ding12cfbe0afb058f3cde5ce625ff4abdf6&53108=bb418&cbdbhh=qwertyuiop&origin=1)
- [WeChat (Simplified Chinese)](https://gw.alipayobjects.com/zos/oceanbase/0a69627f-8005-4c46-be1f-aac7a2b85c13/image/2022-03-01/85d42796-4e22-463a-9658-57402d7b9bc3.png)