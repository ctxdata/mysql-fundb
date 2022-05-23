# FunDB: an experiment MySQL Storage Engine

Why it's called FunDB?
------------------------
This is a custom Storage Engine for experiment purpose. Just for fun.<br>
I'll write a MySQL storage engine step by step, supposed this is a mutli-featured experiment storage engine. 

What feature would it have?
------------------------
I hope a talbe with FunDB will behave like Redis List value, the table will contain only two fields, id and value. 'id' is a typical INT AUTO INCREMENT PRIMARY KEY, while value is a List of Long which may present a stream of IDs for other tables.
|id |messages  |
--- | --- |
|1|12,34,118,874|
|2|443,8080|

So a SQL statement like `INSERT INTO tbl_user_messages VALUE(1, 8081)`, will be turn into an update to the row with `id=1`, and the logical representation of table will be

|id |messages  |
--- | --- |
|1|12,34,118,874,<b>8081</b>|
|2|443,8080|

I know this may seem ridiculous, but I believe the best way to understand a system is to add a new feature into it. I hope I can complete this experiment storage engine in next few months. Just do it!

Step by Step to implement this custom Storage Engine
----------------------------------
Refer to [The birth of FunDB](https://ctxdata.github.io/the-birth-of-fundb/)
