// main.ts
import { serveDir } from "https://deno.land/std@0.214.0/http/file_server.ts";

Deno.serve((req) => serveDir(req, {
  fsRoot: "./public",
  urlRoot: "",
  showDirListing: true,
}));
