package dto.security.video
{
	import mx.collections.ArrayCollection;
	[Bindable]
	[RemoteClass(alias="com.broadengate.vcms.entity.VideoAlbumShipId")]
	public class VideoAlbumShipId
	{
		
		public var albumId:int;
		public var videoId:int;
		public var albumSeq:int;
		public var createTime:Date;
		public function VideoAlbumShipId()
		{
			//TODO: implement function
		}

	}
}