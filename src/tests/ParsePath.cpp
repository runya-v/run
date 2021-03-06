#include <mntent.h>

#include <string>

#include <boost/filesystem.hpp>

#include "Plugin.hpp"


class ParsePath
    : public Plugin
{
public:
    ParsePath()
    {}

    std::string device2point(const std::string& device)
    {
        std::shared_ptr<FILE> fd(fopen("/proc/mounts", "r"), fclose);

        if (fd)
        {
            while (mntent* ent = getmntent(fd.get()))
            {
                std::string d = ent->mnt_fsname;
                std::string p = ent->mnt_dir;

                if (device == d)
                {
                    return p;
                }

                if (device == "gvfs-fuse-daemon" && p.compare(p.length() - 6, 6, "/.gvfs") == 0)
                {
                    return p.substr(p.length());
                }
            }
        }

        return "";
    }

    std::string point2device(const std::string& point)
    {
        std::shared_ptr<FILE> fd(fopen("/proc/mounts", "r"), fclose);

        if (fd)
        {
            while (mntent* ent = getmntent(fd.get()))
            {
                std::string d = ent->mnt_fsname;
                std::string p = ent->mnt_dir;

                if (point == p)
                {
                    return d;
                }
            }
        }

        return "";
    }

    std::string point(const std::string& abspath)
    {
        boost::filesystem::path path(abspath);

        while (!path.empty())
        {
            std::shared_ptr<FILE> fd(fopen("/proc/mounts", "r"), fclose);

            if (fd)
            {
                while (mntent* ent = getmntent(fd.get()))
                {
                    std::string d = ent->mnt_fsname;
                    std::string p = ent->mnt_dir;

                    if (path.string() == p)
                    {
                        return p;
                    }
                }

                path = path.parent_path();
            }
        }

        return "";
    }

    std::string device(const std::string& abspath)
    {
        return point2device(point(abspath));
    }

    std::string dir(const std::string& abspath)
    {
        return abspath.substr(point(abspath).length());
    }


    std::string w_device2point(const std::string& device)
    {
        return device;
    }

    std::string w_point2device(const std::string& point)
    {
        return point;
    }

    std::string w_point(const std::string& abspath)
    {
        return abspath.substr(0, 2);
    }

    std::string w_device(const std::string& abspath)
    {
        return w_point2device(w_point(abspath));
    }


    std::string w_dir(const std::string& abspath)
    {
        return abspath.substr(w_point(abspath).length());
    }

    bool execute()
    {
        namespace bfs = boost::filesystem;
        std::string abspath = "/home/runya/develop/IMEDIA/mediaserver/tests/test_archive_mount_create/mount";

        std::cout << "lin: " << std::endl;
        std::cout << "device: " << device(abspath) << std::endl;
        std::cout << "point: " << point(abspath) << std::endl;
        std::cout << "dir: " << dir(abspath) << std::endl;
        std::cout << "abspath: " << (bfs::path(point(abspath)) /= dir(abspath)).string() << std::endl << std::endl;

        abspath = "c:/home/runya/develop/IMEDIA/mediaserver/tests/test_archive_mount_create/mount";

        std::cout << "win: " << std::endl;
        std::cout << "device: " << w_device(abspath) << std::endl;
        std::cout << "point: " << w_point(abspath) << std::endl;
        std::cout << "dir: " << w_dir(abspath) << std::endl;
        std::cout << "abspath: " << (bfs::path(w_point(abspath)) /= w_dir(abspath)).string() << std::endl << std::endl;

        return true;
    }
} parse_path;
