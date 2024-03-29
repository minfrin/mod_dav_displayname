# mod\_dav\_displayname
Module that makes the name of a resource or a collection available as the DAV:displayname property.

Use with the [mod\_dav\_autoindex](https://github.com/minfrin/mod_dav_autoindex) module to provide human readable filenames in the XML output.

## Download:

RPM Packages are available at
[COPR](https://copr.fedorainfracloud.org/coprs/minfrin/mod_dav_displayname/) for EPEL,
Fedora and OpenSUSE.

```
dnf copr enable minfrin/mod_dav_displayname
dnf install mod_dav_displayname
```

Ubuntu packages are available through
[PPA](https://launchpad.net/~minfrin/+archive/ubuntu/apache2/).

## basic configuration

```
# Basic configuration:
<Location />
  Dav on
  DavDisplayname on
</Location>
```

## example result

```
<?xml version="1.0" encoding="utf-8"?>
<D:multistatus xmlns:D="DAV:" xmlns:ns0="DAV:">
<D:response xmlns:lp1="DAV:">
<D:href>/storage/The%20Lazy%20Dog.txt</D:href>
<D:propstat>
<D:prop>
<lp1:displayname>The Lazy Dog</lp1:displayname>
</D:prop>
<D:status>HTTP/1.1 200 OK</D:status>
</D:propstat>
</D:response>
</D:multistatus>
```
