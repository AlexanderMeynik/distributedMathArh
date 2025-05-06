CREATE TYPE "role" AS ENUM (
  'user',
  'admin'
);

CREATE TYPE "status" AS ENUM (
  'pending',
  'running',
  'completed',
  'failed',
  'archived'
);

CREATE TYPE "iteration_status" AS ENUM (
  'pending',
  'completed',
  'failed'
);

CREATE TYPE "node_status" AS ENUM (
  'active',
  'busy',
  'inactive',
  'error'
);

CREATE TYPE "log_severity" AS ENUM (
  'info',
  'warning',
  'error'
);

CREATE TYPE "operation_type" AS ENUM (
  'generate',
  'solve',
  'mesh'
);

CREATE TABLE "User" (
  "user_id" BIGSERIAL PRIMARY KEY,
  "login" varchar(255) UNIQUE NOT NULL,
  "hashed_password" text NOT NULL,
  "role" role NOT NULL,
  "created_at" timestamp DEFAULT (now()),
  "last_login" timestamp
);

CREATE TABLE "Experiment" (
  "experiment_id" BIGSERIAL PRIMARY KEY,
  "user_id" bigint NOT NULL,
  "status" status NOT NULL,
  "parameters" jsonb NOT NULL,
  "start_time" timestamp,
  "end_time" timestamp,
  "sum_mesh" double PRECISION[]
);

CREATE TABLE "Iteration" (
  "iteration_id" BIGSERIAL PRIMARY KEY,
  "experiment_id" bigint NOT NULL,
  "node_id" bigint,
  "iter_t" operation_type NOT NULL,
  "status" iteration_status NOT NULL DEFAULT 'pending',
  "output_data" jsonb,
  "start_time" timestamp DEFAULT (now()),
  "end_time" timestamp
);

CREATE TABLE "Node" (
  "node_id" BIGSERIAL PRIMARY KEY,
  "ip_address" inet NOT NULL,
  "benchmark_score" bigint [],
  "status" node_status NOT NULL,
  "last_ping" timestamp
);

CREATE TABLE "Log" (
  "log_id" BIGSERIAL PRIMARY KEY,
  "node_id" bigint NULL,
  "severity" log_severity NOT NULL,
  "message" text NOT NULL,
  "timestamp" timestamp DEFAULT (now())
);

ALTER TABLE "Experiment" ADD FOREIGN KEY ("user_id") REFERENCES "User" ("user_id");

ALTER TABLE "Iteration" ADD FOREIGN KEY ("experiment_id") REFERENCES "Experiment" ("experiment_id");

ALTER TABLE "Iteration" ADD FOREIGN KEY ("node_id") REFERENCES "Node" ("node_id");

--ALTER TABLE "Log" ADD FOREIGN KEY ("node_id") REFERENCES "Node" ("node_id");


CREATE OR REPLACE FUNCTION fmt_ts_us(ts timestamp)
    RETURNS text AS $$
SELECT to_char($1, 'YYYY-MM-DD HH24:MI:SS.US');
$$ LANGUAGE sql IMMUTABLE;