About Metis
-------------------------------------------------------------------------------

Metis is an advanced, modular architecture designed to manage structured data migration and visualize differences between snapshots using Protocol Buffers (Protobuf).

Metis Migrator
-------------------------------------------------------------------------------

Migrator is a flexible and extensible migration tool designed to facilitate seamless transitions between different versions of software, databases, or configurations. By utilizing dynamically loaded plugins, Migrator enables users to define custom migration steps and build a structured migration path from one version to another, such as 1.0.0 → 2.0.0.

Key Features: </br>

✅ Dynamic Plugin System – Load migration steps dynamically via plugins.  </br>
✅ Versioned Migrations – Supports step-by-step version transitions. </br>
✅ Custom Migration Paths – Define specific upgrade paths for complex migrations. </br>
✅ Logging & Monitoring – Track migration progress and errors.

![Image](https://github.com/user-attachments/assets/2f77ea2b-22cf-4200-97f9-c78ecbc84427)

Metis Differ
-------------------------------------------------------------------------------

Metis Differ is an advanced comparison tool designed to analyze and visualize differences between two snapshots of data. It provides a tree-based hierarchical view, making it easy to identify changes at different levels, from high-level settings to deeply nested structures.

Key Features: </br> 

✅ Structured Tree View – Displays differences in a hierarchical format, allowing users to expand and collapse nodes for better navigation. </br>
✅ Side-by-Side Comparison – Shows old and new values for each modified field, highlighting changes between snapshots. </br>
✅ Granular Analysis – Supports detailed comparisons across various data structures, including settings, components, layers, and other entities. </br>
✅ Color-Coded Differences – Uses visual cues (e.g., color changes) to differentiate between added, removed, and modified fields. </br>
✅ Version-Aware Comparisons – Aligns with Metis Migrator to compare snapshots across different versions. </br>

![Image](https://github.com/user-attachments/assets/a4e1551e-420c-45c2-96e7-ec62e4922d36)

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