/*
 * Naive test application.
 */

#include "config.h"
#include <czmq.h>
#include <getopt.h>

#define PUBLISH_PERIOD_MSEC 5000
#define BEACON_PUBLISH_PORT 9999 

static bool verbose = false;

static void print_usage(const char *prog)
{
    printf("Usage: %s [-vpuPh]\n", prog);
    puts("  -v --verbose  Print debugging messages\n");
    exit(1);
}

static void parse_opts(int argc, char *argv[])
{
    while (1) {
        static const struct option lopts[] = {
            { "verbose",  0, 0, 'v' },
            { NULL, 0, 0, 0 },
        };
        int c;

        c = getopt_long(argc, argv, "vP:p:u:h", lopts, NULL);

        if (c == -1)
            break;

        switch (c) {
            case 'v':
                verbose = true;
                break;
            default:
                print_usage(argv[0]);
                break;
        }
    }
}

int main(int argc, char *argv[])
{
    parse_opts (argc, argv);
    // Daemon-specific initialization goes here
    zsys_init ();
    zsys_set_logsystem (true);
    // Ctrl-C and SIGTERM will set zsys_interrupted.
    zsys_catch_interrupts ();
    // Show version string
    zsys_info ("This is %s\n", PACKAGE_STRING);

    //  Create listener beacon to receive other servers peers
    zactor_t *listener = zactor_new (zbeacon, NULL);
    assert (listener);
    if (verbose)
        zstr_sendx (listener, "VERBOSE", NULL);
    zsys_info ("Beacon service initialized");

    zsock_send (listener, "si", "CONFIGURE", BEACON_PUBLISH_PORT);
    char *hostname = zstr_recv (listener);
    if (!*hostname) {
        exit(EXIT_FAILURE);
    }
    free (hostname);
    zsys_info ("Beacon service configured");
    // We will listen to anything (empty subscription)
    zsock_send (listener, "sb", "SUBSCRIBE", "", 0);

    // Hash table that holds all the beacon information that we received so far
    zhash_t *peers_hash = zhash_new ();
    assert (peers_hash);

    // Main loop 
    char *ipaddress, *received;
    while (!zsys_interrupted) {
        zstr_recvx (listener, &ipaddress, &received, NULL);
        if (ipaddress) {
            // Remove the first three characters "VP "
            int rc = zhash_insert (peers_hash, received + 3, ipaddress);
            // Print if not known already
            if (rc == 0) {
                printf ("%s - %s\n", received + 3,
                        (const char *) zhash_lookup (peers_hash, received + 3));
            }
        }

        zclock_sleep (10u); /* release to let the OS do something else */
    }
    zstr_free (&ipaddress);
    zstr_free (&received);

    // Write peers hash to file for later use
    zhash_comment (peers_hash, "Peers found in VOST system");
    zhash_save (peers_hash, "peers.txt");
    // Delete hash table
    zhash_destroy (&peers_hash);

    // Tear down the beacon
    zsys_info ("Tear down beacon service");
    // Stop listening 
    zstr_sendx (listener, "UNSUBSCRIBE", NULL);
    zactor_destroy (&listener);
    exit (EXIT_SUCCESS);
}
