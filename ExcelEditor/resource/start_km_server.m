function start_km_server()

%{
try
    jFrame = com.mathworks.mde.desk.MLDesktop.getInstance.getMainFrame;
    jFrame.setVisible(false);
catch e
    disp(['exception 1 ', e.message]);
end
%}

import java.net.ServerSocket
import java.io.*

port = 5001;
server = ServerSocket(port);
disp(['KM server listening on port ', num2str(port)]);

cleanupObj = onCleanup(@() closeServer(server));

%while true
    socket = server.accept();
    disp('Client connected');

    in  = BufferedReader(InputStreamReader(socket.getInputStream()));
    out = PrintWriter(socket.getOutputStream(), true);

    try
        while true
            disp('reading request')
            jline = in.readLine();

            % Client closed connection cleanly
            if isempty(jline)
                disp('empty line')
                break;
            end
            
            vals = sscanf(char(jline), '%f');
            disp(['request received: ', char(jline), ' with ', mat2str(numel(vals)), ' parameters'])
            try
                args = num2cell(vals);        % convert to cell array first
                result = feval(@calc_KM, args{:});
                disp(['returning result ', mat2str(result)]);
                out.println(mat2str(result));
            catch e
                out.println(['ERROR: ', e.message]);
            end
        end
    catch e
        disp(['exception ', e.message]);
    end

    socket.close();
    disp('socket closed');
%end

end

function closeServer(server)
try
    server.close();
    disp('Server socket closed');
catch
end
end
