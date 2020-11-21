#
## History
最早是石油管道数据采集，有如下需求，也形成了如今mqtt的核心
- simple implementation
- quality of service data delivery
- lightweight and bandwidth efficient
- data agnostic 数据不可知性
- continuous session awareness 连续的会话感知
## Basic Concepts
### publish/subscribe
The pub/sub model decouples the client that sends a message (the publisher) from the client or clients that receive the messages (the subscribers), The connection between them is handled by a third component (the broker).  
The decoupling has three dimensions: space, time, and synchronization.
### client/broker
Both publishers and subscribers are MQTT clients.
The broker is responsible for 
- receiving all messages
- filtering the messages
- determining who is subscribed to each message
- sending the message to these subscribed clients
- holds the session data of all clients that have persistent sessions, including subscriptions and missed messages (more details)
- authentication and authorization of clients
## Basic Functionality
### connect
### publish
### subscribe
## Features
### quality of service
MQTT has three Quality of Service (QoS) levels
- At most once (0)
- At least once (1)
- Exactly once (2).
### retained messages
### persistent session
In a persistent session, the broker stores the following information (even if the client is offline).  
- Existence of a session (even if there are no subscriptions).
- All the subscriptions of the client.
- All messages in a Quality of Service (QoS) 1 or 2 flow that the client has not yet confirmed.
- All new QoS 1 or 2 messages that the client missed while offline.
- All QoS 2 messages received from the client that are not yet completely acknowledged.
 When a client requests the server to hold session data, the client is responsible for storing the following information:
- All messages in a QoS 1 or 2 flow, that are not yet confirmed by the broker.
- All QoS 2 messages received from the broker that are not yet completely acknowledged.
### last will and testament
### keep alive