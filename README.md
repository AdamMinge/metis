About Metis
-------------------------------------------------------------------------------

Metis is an advanced, modular architecture designed to manage structured data migration and visualize differences between snapshots using Protocol Buffers (Protobuf).

Metis Migrator
-------------------------------------------------------------------------------

Migrator is a flexible and extensible migration tool designed to facilitate seamless transitions between different versions of software, databases, or configurations. By utilizing dynamically loaded plugins, Migrator enables users to define custom migration steps and build a structured migration path from one version to another, such as 1.0.0 → 2.0.0.

Features
Dynamic Plugin System – Load migration steps dynamically via plugins.
Versioned Migrations – Supports step-by-step version transitions.
Custom Migration Paths – Define specific upgrade paths for complex migrations.
Logging & Monitoring – Track migration progress and errors.

![Image](https://github.com/user-attachments/assets/2f77ea2b-22cf-4200-97f9-c78ecbc84427)

Metis Differ
-------------------------------------------------------------------------------


Metis Plugins
-------------------------------------------------------------------------------
<table>
  <tr>
    <th>Plugin Name</th>
    <th>Description</th>
  </tr>
  <tr>
    <td>diff_v1.0.0</td>
    <td>Add possibility to make diff of snapshots v1.0.0.</td>
  </tr>
  <tr>
    <td>diff_v2.0.0</td>
    <td>Add possibility to make diff of snapshots v2.0.0.</td>
  </tr>
  <tr>
    <td>diff_v3.0.0</td>
    <td>Add possibility to make diff of snapshots v3.0.0.</td>
  </tr>
  <tr>
    <td>migrate_v1.0.0_to_v2.0.0</td>
    <td>Add possibility to make snapshots migration between v1.0.0 and v2.0.0.</td>
  </tr>
  <tr>
    <td>migrate_v2.0.0_to_v3.0.0</td>
    <td>Add possibility to make snapshots migration between v2.0.0 and v3.0.0.</td>
  </tr>
</table>

Others
-------------------------------------------------------------------------------

[News](https://github.com/AdamMinge/metis/blob/master/NEWS.md) - Read the latest updates and release notes for more details on each version's major changes.