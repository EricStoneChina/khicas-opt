# Official KhiCAS baseline

Source: https://www-fourier.univ-grenoble-alpes.fr/~parisse/casio/giacbf.tgz

The author's current Casio source archive was retrieved on 2026-09-10 UTC.
The server reports Last-Modified: 2026-07-31 16:38:44 UTC. The rolling archive
has no release tag; its identity is pinned by SHA-256 in UPSTREAM.json.
The Makefile still labels the add-in 1.8.0; the archive date is not a new
semantic version number.

Source files live at the repository root and are byte-for-byte copies from
that archive in this baseline commit. UPSTREAM.json records every imported
file. Build products, editor backups, author-local publication scripts and
absolute documentation links were excluded. The required libmicropy.a and
the relative iostream -> iostream.new link are retained.

Original copyright and license notices are retained in each source file.
No branding, Chinese UI/help, website or conversion changes from the previous
khicas-opt tree are included in this baseline.
