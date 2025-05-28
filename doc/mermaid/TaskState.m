---
config:
  theme: base
  look: neo
  layout: dagre
---
stateDiagram
  direction TB
  [*] --> Created
  Created --> Queued
  Queued --> Running
  Running --> Succeeded
  Running --> Error
  Succeeded --> Archived
  Error --> Archived
  Archived --> [*]
  Error --> [*]
  Succeeded --> [*]
